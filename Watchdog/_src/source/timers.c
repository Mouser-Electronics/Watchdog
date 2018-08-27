/*
 * timers.c
 *
 * Created: 6/27/2018 12:44:24 PM
 *  Author: braddock.bresnahan
 */ 

#include "timers.h"
#include "driver_init.h"
#include "band.h"
#include "messages.h"
#include "led.h"
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
	durations[TIMER_CONFIG_NORMAL].duration					= NORMAL_TIMER_BAND_NOT_FOUND;
	
	durations[TIMER_CONFIG_NORMAL_TIMEOUT].id				= TIMER_CONFIG_NORMAL_TIMEOUT;
	durations[TIMER_CONFIG_NORMAL_TIMEOUT].duration			= NORMAL_TIMER_TIMEOUT;
	
	durations[TIMER_CONFIG_CONNECTING_DURATION].id			= TIMER_CONFIG_CONNECTING_DURATION;
	durations[TIMER_CONFIG_CONNECTING_DURATION].duration	= CONNECTING_TIMER_DURATION;
	
	durations[TIMER_CONFIG_CONNECTING_COUNTER].id			= TIMER_CONFIG_CONNECTING_COUNTER;
	durations[TIMER_CONFIG_CONNECTING_COUNTER].duration		= CONNECTING_TIMER_COUNTER;
	
	durations[TIMER_CONFIG_DISCONNECTING_TIMEOUT].id		= TIMER_CONFIG_DISCONNECTING_TIMEOUT;
	durations[TIMER_CONFIG_DISCONNECTING_TIMEOUT].duration	= DISCONNECTING_TIMER_TIMEOUT;
	
	durations[TIMER_CONFIG_LOWPOWER].id						= TIMER_CONFIG_LOWPOWER;
	durations[TIMER_CONFIG_LOWPOWER].duration				= LOWPOWER_TIMER_TIMEOUT;
	
	durations[TIMER_CONFIG_LOWPOWER_BUTTON].id				= TIMER_CONFIG_LOWPOWER_BUTTON;
	durations[TIMER_CONFIG_LOWPOWER_BUTTON].duration		= LOWPOWER_TIMER_BUTTON_DURATION;
	
	durations[TIMER_CONFIG_NONE].id							= TIMER_CONFIG_NONE;
	durations[TIMER_CONFIG_NONE].duration					= 0;
	
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
	if (timer == &TIMER0)		return &task0;
	else if (timer == &TIMER1)	return &task1;
	else if (timer == &TIMER2)	return &task2;
	else						return NULL;
}

struct timer_descriptor* GetTimerOfTask(const struct timer_task* const task){
	if (task == &task0)			return &TIMER0;
	else if (task == &task1)	return &TIMER1;
	else if (task == &task2)	return &TIMER2;
	else						return NULL;
}

volatile struct TimerDuration* GetDurationOfTimer(const struct timer_descriptor* const timer){
	if (timer == &TIMER0)		return task0cfg;
	else if (timer == &TIMER1)	return task1cfg;
	else if (timer == &TIMER2)	return task2cfg;
	else						return NULL;
}

volatile struct TimerDuration* GetDurationOfTask(const struct timer_task* const task){
	if (task == &task0)			return task0cfg;
	else if (task == &task1)	return task1cfg;
	else if (task == &task2)	return task2cfg;
	else						return NULL;
}

void BeginTimer(struct timer_descriptor* timer){
	ResetTimer(timer);
	EnableTimer(timer);
}

void EndTimer(struct timer_descriptor* timer){
	DisableTimer(timer);
	ResetTimer(timer);
}

void ConfigTimer(struct timer_descriptor* timer, TimerConfig template){
	if (timer == &TIMER0){
		task0cfg = &durations[template];
		task0cfg->length = task0cfg->duration * TIMER3_DURATION_SEC;
		task0.interval = (uint)task0cfg->duration;
	}
	else if (timer == &TIMER1){
		task1cfg = &durations[template];
		task1cfg->length = task1cfg->duration * TIMER3_DURATION;
		task1.interval = (uint)(task1cfg->duration * SEC_TO_MILSEC);
	}
	else if (timer == &TIMER2){
		task2cfg = &durations[template];
		task2cfg->length = task2cfg->duration * TIMER3_DURATION_SEC;
		task2.interval = (uint)(task2cfg->duration);
	}
	else;
	ResetTimer(timer);
}

void ISR_Timer(const struct timer_task* const task){
	struct EventMessage message;
	struct timer_descriptor* timer;
	
	timer = GetTimerOfTask(task);
	DisableTimer(timer);
	MCUSkip = false; // Run event and update functions after interrupt
	
	message.type = EVENT_TIMER;
	*(struct timer_task**)(message.info + TIMER_EVENT_INFO_SENDER) = (struct timer_task*)task;
	
	//*((const struct timer_task**)(message.info + TIMER_EVENT_INFO_SENDER)) = task;
	QUE_Enqueue(writeEventQueue, &message);
	
	if (timer == &TIMER0 && !_timer_is_started(&TIMER0.device))
		EnableTimer(timer);
}

void ISR_Queue(const struct timer_task* const task){
	static struct timer_task tsk = { .cb = TIMER3_CALLBACK, .mode = TIMER_TASK_ONE_SHOT, .interval = TIMER3_DURATION, };
	static struct QueueInst inst;
	bool end = true; // Becomes false when a queue is not empty (ie. continue to run)
	
	// Handle Motor
	if (!QUE_IsEmpty(&task3)){
		QUE_Dequeue(&task3, &inst);
		switch (inst.type){
			case INST_MOTOR_REST: SetMotor(OFF); SetLED(LEDD, OFF); break;
			case INST_MOTOR_PRIME: /* Not implemented */
			case INST_MOTOR_WEAK: /* Not implemented */
			case INST_MOTOR_SHAKE: /* Not implemented */
			case INST_MOTOR_ACTIVE: SetMotor(ON); SetLED(LEDD, ON); break;
			default: break; // Do nothing
		}
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
		if (end) // If a task has already been added, I shouldn't add another
			timer_add_task(&TIMER3, &tsk);
		end = false;
	}
	// Timer end condition check
	if (end){
		timer_stop(&TIMER3);
	}
}