/*
 * interrupts.c
 *
 * Created: 6/27/2018 12:40:58 PM
 *  Author: braddock.bresnahan
 */ 

#include "watchdog.h"

/// Function Definitions

void InitInterrupts(){
	ext_irq_register(BUTTON, ISR_Button);
	ext_irq_register(ACCEL, ISR_Accelerometer);
}

void ISR_Button(){
	static struct EventMessage message;
	
	DisableInterrupt(BUTTON);
	MCUSkip = false; // Run event and update functions after interrupt
	
	message.info[BUTTON_EVENT_INFO_BUTTON_STATE] = ButtonPressed; // Get button state as soon as possible (so the value doesn't change before it's read)
	message.type = EVENT_BUTTON;
	QUE_Enqueue(writeEventQueue, &message);
	
	if (alarmed) // Button pressed while alarmed stops the alarm
		alarmed = false;
	
	EnableInterrupt(BUTTON);
}

void ISR_Accelerometer(){
	static struct EventMessage message;
	
	DisableInterrupt(ACCEL);
	MCUSkip = false; // Run event and update functions after interrupt
	
	if (!messageQueueInitialized) goto END; // Fixed bug where interrupt fires before the init of I2C
	message.type = EVENT_ACCEL;
	QUE_Enqueue(writeEventQueue, &message);

	/**/END:/**/
	EnableInterrupt(ACCEL);
}