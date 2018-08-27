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

// TIMER0 and TIMER2 tick on seconds (long time range)
// TIMER1 and TIMER3 tick on milliseconds (short time range)
// Short range timers can go up to ~60 seconds
// Short range timers use more power.

// Power Limitations: The MCU wakes on every timer tick, not duration.
// TIMER0 and TIMER2 ticks once per second; while TIMER1 and TIMER3 tick once per millisecond.
// Timers only generate interrupts after duration, not every tick.

/// Macros

// Basic conversion constants
#define SEC_TO_MILSEC	1000
#define MILSEC_TO_SEC	(1 / 1000)

#define MAIN_TIMER_DURATION					1 // Seconds. All of these are debug values. The will be changed in the final build

#define DISPLAY_TIMER_DURATION				3  // Seconds
#define DISPLAY_TIMER_BUTTON_DURATION		4  // Seconds

#define NORMAL_TIMER_BAND_NOT_FOUND			7 // Seconds // Should always be greater than main timer duration
#define NORMAL_TIMER_TIMEOUT				(10 * 60) // In Seconds // 10 minutes

#define CONNECTING_TIMER_DURATION			6  // Seconds // Uses coarse timer
#define CONNECTING_TIMER_COUNTER			60 // In Seconds // Uses fine timer

#define DISCONNECTING_TIMER_TIMEOUT			8 // Seconds

#define LOWPOWER_TIMER_TIMEOUT				10 // Seconds
#define LOWPOWER_TIMER_BUTTON_DURATION		2  // Seconds

#define TIMER3_DURATION		70 // In milliseconds. Used for Motor and LED queue.
#define TIMER3_DURATION_SEC	(SEC_TO_MILSEC / TIMER3_DURATION)
#define TIMER3_CALLBACK		ISR_Queue // The callback for the TIMER3.

#define TIMER_CONFIG_COUNT	(TIMER_CONFIG_NONE - TIMER_CONFIG_MAIN) // The number of different timer configurations.

/// Enums

/*
// Name: TimerConfig
// Desc: The timer configurations that a timer can have (TIMER3's config cannot[should not] change).
*/
typedef enum{
	TIMER_CONFIG_MAIN = 0,					// Main timer configuration
	TIMER_CONFIG_DISPLAY,					// Display timer configuration
	TIMER_CONFIG_DISPLAY_BUTTON,			// Display Button timer configuration
	TIMER_CONFIG_NORMAL,					// Normal timer configuration
	TIMER_CONFIG_NORMAL_TIMEOUT,			// Normal Timeout timer configuration
	TIMER_CONFIG_CONNECTING_DURATION,		// Connecting timer configuration
	TIMER_CONFIG_CONNECTING_COUNTER,		// Connecting Counter timer configuration
	TIMER_CONFIG_DISCONNECTING_TIMEOUT,		// Disconnect Timeout timer configuration
	TIMER_CONFIG_LOWPOWER,					// Low-power timer configuration
	TIMER_CONFIG_LOWPOWER_BUTTON,			// Low-power Button timer configuration
	/*** PUT NEW ENTRIES BETWEEN HERE ***/
	
	/*** PUT NEW ENTRIES BETWEEN HERE ***/
	TIMER_CONFIG_NONE, // No config
} TimerConfig;

/*
// Name: QueueInstruction
// Desc: The possible queue instructions a QueueInst can have.
*/
typedef enum{
	INST_NOP,			// No operation
	INST_MOTOR_REST,	// Stop the motor
	INST_MOTOR_PRIME,	// Prime the motor
	INST_MOTOR_WEAK,	// Start the motor soft
	INST_MOTOR_ACTIVE,	// Start the motor normal
	INST_MOTOR_SHAKE,	// Start the motor hard
	INST_LED_ON,		// Turn on LED
	INST_LED_OFF,		// Turn off LED
} QueueInstruction;

/// Structs

/*
// Name: TimerDuration
// Desc: The configuration/duration of a timer. Also assists in the timer's function.
// Members: (TimerConfig) id: The identity of the timer configuration.
//          (float) duration: The duration of the timer in seconds. Resolution of actual
//                            timer duration changes based on the duration value.
//          (uint)    length: The approximate number of TIMER3 ticks for the duration.
//                            Useful for syncing motor and LED queue to main loop.
*/
struct TimerDuration{
	TimerConfig	id;
	float duration;
	uint length;
};

/*
// Name: QueueInst
// Desc: A motor or LED queue instruction.
// Members: (QueueInstruction) type: The instruction to carry out.
//          (uint)             misc: Other information if needed.
*/
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
// Name: EnableTimer
// Desc: Starts/Restarts the given timer. the timer needs to be disabled before a restart.
// Input: (struct timer_descriptor*) timer: The timer to start/restart.
*/
inline static void EnableTimer(struct timer_descriptor* timer){
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
void BeginTimer(struct timer_descriptor* timer);
void EndTimer(struct timer_descriptor* timer);

/*
// Name: ConfigureTimer
// Desc: Configures the given timer with the given template enum.
// Input: (struct timer_descriptor*) timer: The timer to configure.
//        (TimerConfig)	          template: The name of the configuration to use.
*/
void ConfigTimer(struct timer_descriptor* timer, TimerConfig template);

inline static void ResetTimer(struct timer_descriptor* timer){
	struct timer_task* task = GetTaskOfTimer(timer);
	task->time_label = timer->time;
}

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