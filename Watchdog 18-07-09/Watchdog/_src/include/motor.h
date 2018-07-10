/*
 * motor.h
 *
 * Created: 6/28/2018 8:06:06 AM
 *  Author: braddock.bresnahan
 */ 


#ifndef MOTOR_H_
#define MOTOR_H_

#include "stdtypes.h"
#include "timers.h"
#include "queue.h"

/// Macros

#define MOTOR_GENERIC_BUTTON_UP_ACTION		"-_"
#define MOTOR_DISPLAY_EVENT_ACTION			"-_"

#define MOTOR_CONNECTING_INIT_ACTION		"_----__----_" // "_,,,,,,,,,,_"
#define MOTOR_CONNECTING_CLOSE_ACTION		"_-_-_"

#define MOTOR_DISCONNECTING_INIT_ACTION		"_-_-_-_" // "_,_,_,_"
#define MOTOR_DISCONNECTING_UPDATE_ACTION	"-_" // "_,_"
#define MOTOR_DISCONNECTING_EVENT_ACTION	MOTOR_DISCONNECTING_CLOSE_ACTION // Event is just closing action but for guest band.
#define MOTOR_DISCONNECTING_CLOSE_ACTION	"_--__--__--_" // "_,,__,,__,,_"

#define MOTOR_LOWPOWER_INIT_ACTION			"_---_---_---_"

#define MOTOR_SLEEP_INIT_ACTION				"_-_-_" //"_-----_-----_" // "_,,,,,_,,,,,_"
#define MOTOR_SLEEP_CLOSE_ACTION			"_--_--_"

/// Functions

/*
// Name: MotorEnqueue
// Desc: Queues the given set of instructions to change motor state; and starts, if able,
//       the timer to handle the instructions. Can queue more instructions at any time,
//       even if the timer is already running. The motor queue shares the same timer as
//       the LED queue, though both queues can still run at the same time.
// Input: (const char* const) instructions: A null terminated string with the instructions
//                                          to queue.
// Instructions:
// '_' / '=' / '.' / '%' / '/' - No vibration
// '+' / '*' - Prime vibration (Not setup yet) // Starts the motor to where the motor is running, but the vibration is not felt.
// '-' - Normal vibration // Starts the motor normally
// ',' - Soft vibration (Not setup yet) // Starts the motor to where the vibration is softly felt.
// '^' - Hard vibration (Not setup yet) // Starts the motor to where the vibration is shaking their arm.
*/
void MotorEnqueue(const char* const instructions);
/*
// Name: MotorClearQueue
// Desc: Clears the queue of motor instructions. This function does not stop the timer
//       directly, but will ultimately lead to it stopping if no more instructions are
//       enqueued.
*/
void MotorClearQueue(void);
/*
// Name: MotorIsIdle
// Desc: Returns if the motor instructions are finished and the timer is stopped.
// Return: (bool) True if there are no motor instructions and the timer is stopped, else
//                false.
*/
bool MotorIsIdle(void);
/*
// Name: MotorDelay
// Desc: Delays the motor timer by the given number of milliseconds. The time delayed is
//       floored in increments of MOTOR_INSTRUCTION_TIME. Use this instead of delay_ms.
// Input: (uint) milliseconds: The approximate amount of time to delay the motor timer.
*/
void MotorDelay(uint milliseconds);

#endif /* MOTOR_H_ */