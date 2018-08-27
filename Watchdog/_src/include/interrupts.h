/*
 * interrupts.h
 *
 * Created: 6/27/2018 12:25:06 PM
 *  Author: braddock.bresnahan
 */ 

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include <hal_ext_irq.h>
#include "stdtypes.h"

/// Function Declarations

/*
// Name: InitInterrupts
// Desc: Initializes the external interrupts for the band. Call this function as soon as
//       possible from start.
*/
void InitInterrupts(void);

/*
// Name: EnableInterrupt
// Desc: Enables the interrupt on the given pin.
// Input: (uint) pin: The pin to enable interrupts on.
*/
inline void EnableInterrupt(uint pin){
	ext_irq_enable(pin);
}
/*
// Name: DisableInterrupt
// Desc: Disables the interrupt on the given pin.
// Input: (uint) pin: the pin to disable interrupts on.
*/
inline void DisableInterrupt(uint pin){
	ext_irq_disable(pin);
}

/*
// Name: ISR_Button
// Desc: The interrupt handler for button events.
*/
void ISR_Button(void);
/*
// Name: ISR_Accelerometer
// Desc: The interrupt handler for accelerometer events.
*/
void ISR_Accelerometer(void);


#endif /* INTERRUPTS_H_ */