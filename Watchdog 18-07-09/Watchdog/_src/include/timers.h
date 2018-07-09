/*
 * timers.h
 *
 * Created: 6/27/2018 12:24:33 PM
 *  Author: braddock.bresnahan
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_

#include <hal_timer.h>
#include "queue.h"
#include "stdtypes.h"

// Timer granularity above 10 seconds is 10 seconds. (10, 20, 30, etc.); No 15s and the like.

/// Macros

#define CLOCK_MILSEC_MULT	3.90625 // Cycles per 1 millisecond (based on 4000 MHz clock with prescale of 1024)
#define CLOCK_SEC_MULT		(1000 * CLOCK_MILSEC_MULT) // Clock cycles per 1 second

#define MAIN_TIMER_DURATION						1 // Seconds. All of these are debug values. The will be changed in the final build

#define DISPLAY_TIMER_DURATION					3  // Seconds
#define DISPLAY_TIMER_BUTTON_DURATION			4  // Seconds

#define NORMAL_TIMER_DURATION					(3 * 60) // Seconds // 3 minutes

#define CONNECTING_TIMER_DURATION				4  // Seconds
#define CONNECTING_TIMER_TIMEOUT				(5 * 60) // In Seconds // 5 minutes

#define DISCONNECTING_TIMER_TIMEOUT				10 // Seconds

#define LOWPOWER_TIMER_DURATION					10 // Seconds
#define LOWPOWER_TIMER_BUTTON_DURATION			2  // Seconds

#define TIMER_LENGTH		(70 * CLOCK_MILSEC_MULT)
#define TIMER_CALLBACK		ISR_Queue

#define TIMER_CONFIG_COUNT			(TIMER_CONFIG_NONE - TIMER_CONFIG_MAIN)
#define TIMER_CONFIG_MAX_DURATION	10 // In seconds

/// Enums

typedef enum{
	TIMER_CONFIG_MAIN = 0,
	TIMER_CONFIG_DISPLAY,
	TIMER_CONFIG_DISPLAY_BUTTON,
	TIMER_CONFIG_NORMAL,
	TIMER_CONFIG_CONNECTING,
	TIMER_CONFIG_CONNECTING_TIMEOUT,
	TIMER_CONFIG_DISCONNECTING_TIMEOUT,
	TIMER_CONFIG_LOWPOWER,
	TIMER_CONFIG_LOWPOWER_BUTTON,
	/*** PUT NEW ENTRIES BETWEEN HERE ***/
	
	/*** PUT NEW ENTRIES BETWEEN HERE ***/
	TIMER_CONFIG_NONE,
} TimerConfig;

typedef enum{
	INST_NOP,
	INST_MOTOR_REST,
	INST_MOTOR_WEAK,
	INST_MOTOR_ACTIVE,
	INST_MOTOR_SHAKE,
	INST_LED_ON,
	INST_LED_OFF,
} QueueInstruction;

/// Structs

struct TimerDuration{
	TimerConfig	id;			// The identity of the duration
	float duration;			// In seconds
	uint cycles;			// Number of cycles for the duration
	uint length;			// The approximate number of TIMER3 cycles for the duration
	bool overtime;			// If the duration is longer than the timer can handle
	uint count;				// Count for when the duration is in overtime
	uint countCap;			// The cap of the count. Fired event when count is equal to this number.
};

struct QueueInst{
	QueueInstruction type;
	uint misc;
};

/// Function Declarations

/*
// Name: InitTimers
// Desc: Initializes the timers for the band. Initializes four TC timers in total; Three
//       for main band functions and one for motor and LED queues.
*/
void InitTimers(void);

/*
// Name: GetTaskOfTimer
// Desc: Returns the task associated with the given timer.
// Input: (const struct timer_descriptor* const) timer: The timer to find the task of.
// Return: (struct timer_task*) The task associated with the timer.
*/
struct timer_task* GetTaskOfTimer(const struct timer_descriptor* const timer);
/*
// Name: GetTimerOfTask
// Desc: Returns the timer associated with the given task.
// Input: (const struct timer_task* const) task: The task to find the timer of.
// Return: (struct timer_descriptor*) The timer associated with the task.
*/
struct timer_descriptor* GetTimerOfTask(const struct timer_task* const task);

/*
// Name: GetDurationOfTimer
// Desc: Returns the timer duration associated with the given timer.
// Input: (const struct timer_descriptor* const) timer: The timer to find the duration of.
// Return: (volatile struct TimerDuration*) The duration associated with the timer.
*/
volatile struct TimerDuration* GetDurationOfTimer(const struct timer_descriptor* const timer);
/*
// Name: GetDurationOfTask
// Desc: Returns the timer duration associated with the given task.
// Input: (const struct timer_task* const) task: The task to find the duration of.
// Return: (volatile struct TimerDuration*) The duration associated with the task.
*/
volatile struct TimerDuration* GetDurationOfTask(const struct timer_task* const task);

/*
// Name: ResetTimerCount
// Desc: Resets the count of the given timer.
// Input: (struct timer_descriptor*) timer: The timer to reset the count of.
*/
inline static void ResetTimerCount(struct timer_descriptor* timer){
	hri_tccount16_write_COUNT_reg(timer->device.hw, 0);
	//GetTaskOfTimer(timer)->time_label = timer->time;
}
/*
// Name: EnableTimer
// Desc: Starts/Restarts the given timer. the timer needs to be disabled before a restart.
// Input: (struct timer_descriptor*) timer: The timer to start/restart.
*/
inline static void EnableTimer(struct timer_descriptor* timer){
	ResetTimerCount(timer);
	timer_start(timer);
}
/*
// Name: DisableTimer
// Desc: Stops the given timer.
// Input: (struct timer_descriptor*) timer: The timer to stop.
*/
inline static void DisableTimer(struct timer_descriptor* timer){
	timer_stop(timer);
}

/*
// Name: ConfigureTimer
// Desc: Configures the given timer with the given template enum.
// Input: (struct timer_descriptor*) timer: The timer to configure.
//        (TimerConfig)	          template: The name of the configuration to use.
*/
void ConfigTimer(struct timer_descriptor* timer, TimerConfig template);

/*
// Name: ISR_Timer
// Desc: The interrupt handler for TIMER0, TIMER1 and TIMER2. Used for important timer
//       events.
*/
void ISR_Timer(const struct timer_task* const task);
/*
// Name: ISR_Queue
// Desc: The interrupt handler for TIMER3. Used for motor and LED queues.
*/
void ISR_Queue(const struct timer_task* const task);

/// Global Variables

/*
// Name: durations
// Desc: The duration configurations timers can choose from.
*/
struct TimerDuration durations[TIMER_CONFIG_COUNT];
/*
// Name: task0
// Desc: The task associated with TIMER0.
*/
struct timer_task task0;
/*
// Name: task1
// Desc: The task associated with TIMER1.
*/
struct timer_task task1;
/*
// Name: task2
// Desc: The task associated with TIMER2.
*/
struct timer_task task2;
/*
// Name: task0cfg
// Desc: The duration configuration associated with task0.
*/
volatile struct TimerDuration* task0cfg;
/*
// Name: task1cfg
// Desc: The duration configuration associated with task1.
*/
volatile struct TimerDuration* task1cfg;
/*
// Name: task2cfg
// Desc: The duration configuration associated with task2.
*/
volatile struct TimerDuration* task2cfg;

/*
// Name: task3
// Desc: A queue for holding motor instructions.
*/
volatile struct Queue task3;
/*
// Name: task4
// Desc: A queue for holding LED instructions.
*/
volatile struct Queue task4;

#endif /* TIMERS_H_ */