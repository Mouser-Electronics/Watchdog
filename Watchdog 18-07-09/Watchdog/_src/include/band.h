/*
 * band.h
 *
 * Created: 6/27/2018 12:04:37 PM
 *  Author: braddock.bresnahan
 */ 


#ifndef BAND_H_
#define BAND_H_

#include <stdbool.h>
#include "queue.h"
#include "array.h"
#include "stdtypes.h"
#include "modes.h"
#include "timers.h"

/// Macros

#define STARTING_BANDRANGE	BANDRANGE_SHORT // The default starting band range.

/// Enums

/*
// Name: BandRange
// Desc: The possible bluetooth ranges for the band.
*/
typedef enum{
	BANDRANGE_NONE,		// 0 meters
	BANDRANGE_SHORT,	// About 10 meters
	BANDRANGE_MEDIUM,	// About 30~40 meters
	BANDRANGE_LONG,		// About 60~80 meters
} BandRange;

/// Function Declarations

/*
// Name: BandMain
// Desc: The main function of the MCU. Handles events and updates. Puts the MCU to sleep
//       after completion.
*/
void BandMain(void);
/*
// Name: HandleUpdates
// Desc: Updates the band using the update function of the current band mode.
*/
void HandleUpdates(void);

/* These Next Functions Have Yet To Be Done */

/*
// Name: ForceBreakAwak
// Desc: Disconnects the band from everything immediately with no notifications, updating,
//       initializing or closing anything.
*/
void ForceBreakAway(void);
/*
// Name: FireAlarm
// Desc: Not a fire alarm. Fires the alarm event for the band.
*/
void FireAlarm(void);

/*
// Name: LEDEnqueue
// Desc: Queues the given set of instructions to change the given LED's state; and starts,
//       if able, the timer to handle the instructions. Can queue more instructions at any
//       time, even if the timer is already running. The motor queue shares the same timer
//       as the LED queue, though both queues can still run at the same time.
// Input: (const char* const) instructions: A null terminated string with the instructions
//                                          to queue.
// Instructions:
// '_' / '=' / '.' / '%' / '/' - LED off
// '-' / '+' / ',' / '^' / '*' - LED on
*/
void LEDEnqueue(uint pin, const char* const instructions);
/*
// Name: LEDClearQueue
// Desc: Clears the queue of LED instructions. This function does not stop the timer
//       directly, but will ultimately lead to it stopping if no more instructions are
//       enqueued.
*/
void LEDClearQueue(void);
/*
// Name: MotorIsIdle
// Desc: Returns if the motor instructions are finished and the timer is stopped.
// Return: (bool) True if there are no motor instructions and the timer is stopped, else
//                false.
*/
bool LEDIsIdle(void);
/*
// Name: MotorDelay
// Desc: Delays the LED timer by the given number of milliseconds. The time delayed is
//       floored in increments of MOTOR_INSTRUCTION_TIME. Use this instead of delay_ms.
// Input: (uint) milliseconds: The approximate amount of time to delay the LED timer.
*/
void LEDDelay(uint milliseconds);

/// Global Variables

/*
// Name: MCUBusy
// Desc: A variable that is incremented every time an interrupt starts and decremented
//       every time an interrupt finishes. That means that if an interrupt is occurring,
//       the value will be non-zero; and if no interrupt is occurring, the value will be
//       zero. Used in BandMain to prevent cutting off interrupts early.
*/
volatile uint MCUBusy;
/*
// Name: MCUSkip
// Desc: A variable that is set to true when the current interrupt event is not important
//       enough to run an entire event and update handling. Used in BandMain to skip over
//       the handling functions. The variable is overridden by a timer interrupt and
//       ignored if there are events queued to be handled.
*/
volatile bool MCUSkip;

/* NEEDS TO BE COMMENTED */
// Wait for comment until after bluetooth is functional.

volatile uint32* host; // The host address
volatile struct Array guests; // An array of guest handles
volatile bool isHost;
volatile bool connected;
// Name: bandRange
// Desc: The current bluetooth range of the band.
volatile BandRange bandRange;
// Name: button
// Desc: The current state of the button. True is pressed and false is not pressed.
//       Updated from event handling.
volatile bool button;
// Name: led
// Desc: The current main led of the band. Changes depending on host/guest/free status.
volatile int led;


#endif /* BAND_H_ */