/*
 * pin.h
 *
 * Created: 7/13/2018 9:30:40 AM
 *  Author: braddock.bresnahan
 */ 

#ifndef PIN_H_
#define PIN_H_

#include <stdbool.h>
#include "stdtypes.h"

// There is no PWM for this setup

/// Macros

#define ON	true
#define OFF	false

// Components of LEDMask
#define LEDD	0b10000000 // ATSAMC21N-XPRO debug LED
#define LEDS	0b00000001 // Short LED
#define LEDM	0b00000010 // Medium LED
#define LEDL	0b00000100 // Long LED
#define LEDR	0b00001000 // Red LED
#define LEDG	0b00010000 // Blue LED
#define LEDB	0b00100000 // Green LED
#define LEDW	(LEDR | LEDG | LEDB) // White
#define LEDRNG	(LEDS | LEDM | LEDL) // All range LEDs
#define LEDALL	(LEDS|LEDM|LEDL|LEDR|LEDG|LEDB)
#define LEDNONE	0

#define LED_GENERAL_BLINK_LENGTH	500 // In milliseconds

/// Functions

/*
// Name: GetLED
// Desc: Gets the current state of the given ledMask.
// Input: (uint8) ledMask: The LEDs to get the state of.
// Return: (bool) True if at least one LED in mask is on, else false.
*/
bool GetLED(uint8 ledMask);

/*
// Name: SetLED
// Desc: Sets the given LEDs to the given state.
// Input: (uint8) ledMask: The LEDs to set.
//        (bool)    state: The state to set the LEDs.
*/
void SetLED(uint8 ledMask, bool state);

/*
// Name: GetActiveLEDs
// Desc: Returns an LED mask with all active LEDs.
// Return: (uint8) An LED mask with all active LEDs.
*/
uint8 GetActiveLEDs(void);

/*
// Name: LEDEnqueue
// Desc: Queues the given set of instructions to change the given LED's state; and starts,
//       if able, the timer to handle the instructions. Can queue more instructions at any
//       time, even if the timer is already running. The motor queue shares the same timer
//       as the LED queue, though both events can still run at the same time.
// Input: (uint8)                      led: The led(s) to queue instructions for. 
//        (const char* const) instructions: A c-string with the instructions to queue.
// Instructions:
// '_' / '=' / '.' / '%' / '/' - LED off
// '-' / '+' / ',' / '^' / '*' - LED on
*/
void LEDEnqueue(uint8 ledMask, const char* const instructions);

/*
// Name: LEDClearQueue
// Desc: Clears the queue of LED instructions. This function does not stop the timer
//       directly, but will ultimately lead to it stopping since no more instructions are
//       queued.
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
//       floored in increments of MOTOR_INSTRUCTION_TIME. Use this instead of delay_ms
//       for LED delays.
// Input: (uint) milliseconds: The approximate amount of time to delay the LED timer.
*/
void LEDDelay(uint milliseconds);
/*
// Name: LEDBlink
// Desc: Blinks the LED once for the given length of time (ON and OFF).
// Input: (uint8) ledMask: The pins to blink.
//        (uint)   length: The length of time, in milliseconds, the LED should be on then
//                         off.
*/
void LEDBlink(uint8 ledMask, uint length);
/*
// Name: LEDBlinkAdv
// Desc: Blinks the given LEDs as many times as can fit in the duration in increments of
//       2 lengths (ON and OFF). Can go beyond duration.
// Input: (uint8)  ledMask: The LEDs to blink.
//        (uint)    length: The blink length in milliseconds (ON and OFF).
//        (float) duration: The length of time to be blinking.
*/
void LEDBlinkAdv(uint8 ledMask, uint length, float duration);

/// Global Variables

/*
// Name: led
// Desc: The current main led mask of the band. Changes depending on host/guest/free status.
*/
volatile uint led;

#endif /* PIN_H_ */