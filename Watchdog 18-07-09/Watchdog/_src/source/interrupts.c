/*
 * interrupts.c
 *
 * Created: 6/27/2018 12:40:58 PM
 *  Author: braddock.bresnahan
 */ 

#include "interrupts.h"
#include "driver_init.h"
#include "boardfuncs.h"
#include "band.h"
#include "modes.h"
#include "messages.h"
#include "power.h"

/// Function Definitions

void InitInterrupts(){
	ext_irq_register(BUTTON, ISR_Button);
	ext_irq_register(ACCEL, ISR_Accelerometer);
}

void ISR_Button(){
	static struct EventMessage message;
	static bool state = false;
	
	/**/BEGIN:/**/
	DisableInterrupt(BUTTON);
	MCUBusy++;
	
	state = ButtonPressed;
	message.eventType = EVENT_BUTTON;
	message.eventBandState = (uint8)bandState;
	message.eventInfo[0] = state;
	QUE_Enqueue(writeEventQueue, &message);
	
	/**/END:/**/
	EnableInterrupt(BUTTON);
	MCUBusy--;
}

void ISR_Accelerometer(){
	/**/BEGIN:/**/
	DisableInterrupt(ACCEL);
	MCUBusy++;
	
	switch (bandState){
		case BANDSTATE_DISPLAY:			break;
		case BANDSTATE_NORMAL:			EventNormal(); break;
		case BANDSTATE_CONNECTING:		break;
		case BANDSTATE_DISCONNECTING:	break;
		case BANDSTATE_LOWPOWER:		break;
		case BANDSTATE_SLEEP:			EventSleep(); break;
		default: break;
	}

	/**/END:/**/
	EnableInterrupt(ACCEL);
	MCUBusy--;
}