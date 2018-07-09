/*
 * timers.c
 *
 * Created: 6/27/2018 12:44:24 PM
 *  Author: braddock.bresnahan
 */ 

#include "timers.h"
#include "driver_init.h"
#include "boardfuncs.h"
#include "band.h"
#include "messages.h"
#include "power.h"
#include "motor.h"

/// Function Definitions

void InitTimers()
{
	/// Configurations/Durations	
	durations[TIMER_CONFIG_MAIN].id							= TIMER_CONFIG_MAIN;
	durations[TIMER_CONFIG_MAIN].duration					= MAIN_TIMER_DURATION;
	
	durations[TIMER_CONFIG_DISPLAY].id						= TIMER_CONFIG_DISPLAY;
	durations[TIMER_CONFIG_DISPLAY].duration				= DISPLAY_TIMER_DURATION;
	
	durations[TIMER_CONFIG_DISPLAY_BUTTON].id				= TIMER_CONFIG_DISPLAY_BUTTON;
	durations[TIMER_CONFIG_DISPLAY_BUTTON].duration			= DISPLAY_TIMER_BUTTON_DURATION;
	
	durations[TIMER_CONFIG_NORMAL].id						= TIMER_CONFIG_NORMAL;
	durations[TIMER_CONFIG_NORMAL].duration					= NORMAL_TIMER_DURATION;
	
	durations[TIMER_CONFIG_CONNECTING].id					= TIMER_CONFIG_CONNECTING;
	durations[TIMER_CONFIG_CONNECTING].duration				= CONNECTING_TIMER_DURATION;
	
	durations[TIMER_CONFIG_CONNECTING_TIMEOUT].id			= TIMER_CONFIG_CONNECTING_TIMEOUT;
	durations[TIMER_CONFIG_CONNECTING_TIMEOUT].duration		= CONNECTING_TIMER_TIMEOUT;
	
	durations[TIMER_CONFIG_DISCONNECTING_TIMEOUT].id		= TIMER_CONFIG_DISCONNECTING_TIMEOUT;
	durations[TIMER_CONFIG_DISCONNECTING_TIMEOUT].duration	= DISCONNECTING_TIMER_TIMEOUT;
	
	durations[TIMER_CONFIG_LOWPOWER].id						= TIMER_CONFIG_LOWPOWER;
	durations[TIMER_CONFIG_LOWPOWER].duration				= LOWPOWER_TIMER_DURATION;
	
	durations[TIMER_CONFIG_LOWPOWER_BUTTON].id				= TIMER_CONFIG_LOWPOWER_BUTTON;
	durations[TIMER_CONFIG_LOWPOWER_BUTTON].duration		= LOWPOWER_TIMER_BUTTON_DURATION;
	
	durations[TIMER_CONFIG_NONE].id							= TIMER_CONFIG_NONE;
	durations[TIMER_CONFIG_NONE].duration					= 0;
	
	for (int i = 0; i < TIMER_CONFIG_COUNT; i++){
		if (durations[i].duration >= TIMER_CONFIG_MAX_DURATION){
			durations[i].cycles = TIMER_CONFIG_MAX_DURATION * CLOCK_SEC_MULT;
			durations[i].overtime = true;
			durations[i].count = 0;
			durations[i].countCap = (uint)durations[i].duration / TIMER_CONFIG_MAX_DURATION;
			durations[i].length = TIMER_CONFIG_MAX_DURATION * durations[i].countCap * (CLOCK_SEC_MULT / TIMER_LENGTH);
		}
		else{
			durations[i].cycles = (uint)(durations[i].duration * CLOCK_SEC_MULT);
			durations[i].length = durations[i].duration * (CLOCK_SEC_MULT / TIMER_LENGTH);
			durations[i].overtime = false;
			durations[i].count = 0;
			durations[i].countCap = 0;
		}
	}
	
	/// Tasks
	task0.cb = task1.cb = task2.cb = ISR_Timer;
	task0.mode = task1.mode = task2.mode = TIMER_TASK_REPEAT;
	task0.interval = task1.interval = task2.interval = 1;
	
	/// Action Queues
	QUE_InitQueue(&task3, sizeof(struct QueueInst));
	QUE_InitQueue(&task4, sizeof(struct QueueInst));
	
	/// Timers
	timer_add_task(&TIMER0, &task0);
	timer_add_task(&TIMER1, &task1);
	timer_add_task(&TIMER2, &task2);
	// Configure main timer
	ConfigTimer(&TIMER0, TIMER_CONFIG_MAIN);
}

struct timer_task* GetTaskOfTimer(const struct timer_descriptor* const timer){
	if (timer == &TIMER0)
	return &task0;
	else if (timer == &TIMER1)
	return &task1;
	else if (timer == &TIMER2)
	return &task2;
	else
	return NULL;
}

struct timer_descriptor* GetTimerOfTask(const struct timer_task* const task){
	if (task == &task0)
	return &TIMER0;
	else if (task == &task1)
	return &TIMER1;
	else if (task == &task2)
	return &TIMER2;
	else
	return NULL;
}

volatile struct TimerDuration* GetDurationOfTimer(const struct timer_descriptor* const timer){
	if (timer == &TIMER0)
		return task0cfg;
	else if (timer == &TIMER1)
		return task1cfg;
	else if (timer == &TIMER2)
		return task2cfg;
	else
		return NULL;
}

volatile struct TimerDuration* GetDurationOfTask(const struct timer_task* const task){
	if (task == &task0)
		return task0cfg;
	else if (task == &task1)
		return task1cfg;
	else if (task == &task2)
		return task2cfg;
	else
		return NULL;
}

void ConfigTimer(struct timer_descriptor* timer, TimerConfig template){
	static volatile struct TimerDuration** cfg;
	
	if (timer == &TIMER0)
		cfg = &task0cfg;
	else if (timer == &TIMER1)
		cfg = &task1cfg;
	else if (timer == &TIMER2)
		cfg = &task2cfg;
	else
		cfg = NULL;
	*cfg = &durations[template];
	(*cfg)->count = 0;
	timer_set_clock_cycles_per_tick(timer, (*cfg)->cycles);
}

void ISR_Timer(const struct timer_task* const task){
	static struct EventMessage message;
	static struct timer_descriptor* timer;
	static volatile struct TimerDuration* dur;
	
	/**/BEGIN:/**/
	MCUBusy++;
	MCUSkip = false; // Overrides an MCUSkip order
	
	timer = GetTimerOfTask(task);
	DisableTimer(timer);
	
	dur = GetDurationOfTask(task);
	if (dur->overtime){
		dur->count++;
		if (dur->count < dur->countCap){ // Don't fire event yet
			EnableTimer(timer);
			MCUSkip = true;
			goto END;
		}
		else{ // Fire event
			dur->count = 0;
		}
	}
	
	message.eventType = EVENT_TIMER;
	message.eventBandState = (uint8)bandState;
	message.eventInfo[0] = EVENT_TIMER_EXPIRED;
	*(struct timer_task**)(message.eventInfo + 1) = (struct timer_task*)task;
	QUE_Enqueue(&writeEventQueue, &message);
	
	/**/END:/**/
	if (timer == &TIMER0 && !_timer_is_started(&TIMER0.device))
		EnableTimer(timer);
	MCUBusy--;
}

void ISR_Queue(const struct timer_task* const task){
	static struct timer_task tsk = { .cb = ISR_Queue, .mode = TIMER_TASK_ONE_SHOT, .interval = 1, };
	static struct QueueInst inst;
	bool end = true; // Becomes false when a queue is not empty (ie. continue to run)
	
	/**/BEGIN:/**/
	MCUBusy++;
	MCUSkip = true;
	
	// Handle Motor
	if (!QUE_IsEmpty(&task3)){
		QUE_Dequeue(&task3, &inst);
		switch (inst.type){
			case INST_MOTOR_REST: SetMotor(OFF); break;
			case INST_MOTOR_WEAK: /* Not implimented */ break;
			case INST_MOTOR_ACTIVE: SetMotor(ON); break;
			case INST_MOTOR_SHAKE: /* Not implimented */ break;
			default: break; // Do nothing
		}
		//hri_tccount16_write_COUNT_reg(TIMER3.device.hw, 0); // Not needed (I think)
		timer_set_clock_cycles_per_tick(&TIMER3, TIMER_LENGTH);
		timer_add_task(&TIMER3, &tsk);
		end = false;
	}
	// Handle LED
	if (!QUE_IsEmpty(&task4)){
		QUE_Dequeue(&task4, &inst);
		switch (inst.type){
			case INST_LED_ON: SetLED(inst.misc, ON); break;
			case INST_LED_OFF: SetLED(inst.misc, OFF); break;
			default: break; // Do nothing
		}
		//hri_tccount16_write_COUNT_reg(TIMER3.device.hw, 0); // Not needed (I think)
		timer_set_clock_cycles_per_tick(&TIMER3, TIMER_LENGTH);
		if (end)
			timer_add_task(&TIMER3, &tsk);
		end = false;
	}
	// Timer end condition check
	if (end){
		timer_stop(&TIMER3);
	}
	
	/**/END:/**/
	MCUBusy--;
}