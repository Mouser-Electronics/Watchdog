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

#define TIMER_DURATION		70 // In milliseconds. Used for Motor and LED queue.
#define TIMER_LENGTH		(TIMER_DURATION * CLOCK_MILSEC_MULT) // The queue timer duration in clock cycles
#define TIMER_CALLBACK		ISR_Queue // The callback for the queue timer.

#define TIMER_CONFIG_COUNT			(TIMER_CONFIG_NONE - TIMER_CONFIG_MAIN) // The number of different timer configurations.
#define TIMER_CONFIG_MAX_DURATION	10 // In seconds. Limited by hardware. Should never go above 16. Will affect the timer resolution of durations longer than itself.

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
	TIMER_CONFIG_CONNECTING,				// Connecting timer configuration
	TIMER_CONFIG_CONNECTING_TIMEOUT,		// Connecting Timeout timer configuration
	TIMER_CONFIG_DISCONNECTING_TIMEOUT,		// Disconnect Timeout timer configuration
	TIMER_CONFIG_LOWPOWER,					// Lowpower timer configuration
	TIMER_CONFIG_LOWPOWER_BUTTON,			// Lowpower Button timer configuration
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
//          (uint)    cycles: The number of cycles for the duration. (duration * CLOCK_SEC_MULT)
//          (uint)    length: The approximate number of TIMER3 ticks for the duration.
//                            Useful for syncing motor and LED queue to main loop.
//          (bool)  overtime: If the duration is longer than a timer can handle; Above
//                            TIMER_CONFIG_MAX_DURATION. When in overtime, not every timer
//                            tick causes an event.
//          (uint)     count: The count for when the duration is in overtime. Increments
//                            every timer tick when in overtime.
//          (uint)  countCap: The stopping point of the count. When count meets this
//                            value, the event is triggered.
*/
struct TimerDuration{
	TimerConfig	id;
	float duration;
	uint cycles;
	uint length;
	bool overtime;
	uint count;
	uint countCap;
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
// Name: ResetTimerCount / ResetTimer
// Desc: Resets the count of the given timer.
// Input: (struct timer_descriptor*) timer: The timer to reset the count of.
*/
inline static void ResetTimerCount(struct timer_descriptor* timer){
	hri_tccount16_write_COUNT_reg(timer->device.hw, 0);
	//GetTaskOfTimer(timer)->time_label = timer->time;
}
inline static void ResetTimer(struct timer_descriptor* timer){
	ResetTimerCount(timer);
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