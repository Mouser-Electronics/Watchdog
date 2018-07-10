/*
 * messages.c
 *
 * Created: 6/28/2018 7:57:25 AM
 *  Author: braddock.bresnahan
 */ 

#include "messages.h"
#include "driver_init.h"
#include "band.h"
#include "boardfuncs.h"
#include "modes.h"
#include "timers.h"
#include "motor.h"

/// Function Definitions

void InitEvents(){
	readEventQueue = &eventMessageQueues[0];
	writeEventQueue = &eventMessageQueues[1];
	QUE_InitQueue(readEventQueue, sizeof(struct EventMessage));
	QUE_InitQueue(writeEventQueue, sizeof(struct EventMessage));
}

void HandleEvents(){
	static struct EventMessage message;
	static void* miscptr = NULL;
	static volatile struct Queue* temp;
	
	// Switch Buffers
	temp = writeEventQueue;
	while (MCUBusy); // Make sure no interrupts are happening
	writeEventQueue = readEventQueue;
	readEventQueue = temp;
	
	while (QUE_Dequeue(readEventQueue, &message)){
		switch (message.eventType){
			case EVENT_BUTTON:
			if (message.eventInfo[BUTTON_EVENT_INFO_BUTTON_STATE] == button) break; // Double event (not supposed to happen)
			else button = message.eventInfo[BUTTON_EVENT_INFO_BUTTON_STATE]; // Set button
			if (button){ // Button Down
				switch (message.eventBandState){ // Band state
					case BANDSTATE_DISPLAY:			EventDisplay(); break;
					case BANDSTATE_NORMAL:			CloseNormal(); InitDisplay(); break;
					case BANDSTATE_CONNECTING:		ResetTimerCount(&TIMER1); break;
					case BANDSTATE_DISCONNECTING:	break;
					case BANDSTATE_LOWPOWER:		EventLowPower(); break;
					case BANDSTATE_SLEEP:			/* Wake up */ break; // Should be handled by the accelerometer, but just in case...
					default: break;
				}
			}
			else{ // Button Up
				//MotorEnqueue(MOTOR_GENERIC_BUTTON_UP_ACTION);
				switch (message.eventBandState){ // Band state
					case BANDSTATE_DISPLAY:			EventDisplay(); break;
					case BANDSTATE_NORMAL:			break;
					case BANDSTATE_CONNECTING:		break;
					case BANDSTATE_DISCONNECTING:	break;
					case BANDSTATE_LOWPOWER:		break;
					case BANDSTATE_SLEEP:			/* Wake up */ break;
					default: break;
				}
			}
			break;
			case EVENT_TIMER:
			switch (message.eventInfo[TIMER_EVENT_INFO_TIMER_EVENT]){
				case EVENT_TIMER_EXPIRED:
				miscptr = &message.eventInfo[TIMER_EVENT_INFO_SENDER]; // pointer to start of message module
				if (*(struct timer_task**)miscptr == &task0){ // MainTimerTimeout
					switch (message.eventBandState){ // Actions not in update functions
						case BANDSTATE_DISPLAY:			if (button) ResetTimerCount(&TIMER1); break;
						case BANDSTATE_NORMAL:			break;
						case BANDSTATE_CONNECTING:		break;
						case BANDSTATE_DISCONNECTING:	break;
						case BANDSTATE_LOWPOWER:		break;
						case BANDSTATE_SLEEP:			break;
						default: break;
					}
				}
				else if (*(struct timer_task**)miscptr == &task1){ // Standard Mode timeout (usually)
					switch (message.eventBandState){
						case BANDSTATE_DISPLAY:			CloseDisplay();	InitNormal(); break;
						case BANDSTATE_NORMAL:			break;
						case BANDSTATE_CONNECTING:		CloseConnecting(); InitDisplay(); break;
						case BANDSTATE_DISCONNECTING:	CloseDisconnecting(); ForceBreakAway(); InitDisplay(); break; // Disconnect failed
						case BANDSTATE_LOWPOWER:		break;
						case BANDSTATE_SLEEP:			break;
						default: break;
					}
				}
				else if (*(struct timer_task**)miscptr == &task2){ // Special Mode timeout (usually)
					switch (message.eventBandState){
						case BANDSTATE_DISPLAY:			CloseDisplay(); if (connected) InitDisconnecting(); else InitConnecting(); break; // button held event
						case BANDSTATE_NORMAL:			CloseNormal(); InitSleep(); break; // Inactive timeout event
						case BANDSTATE_CONNECTING:		CloseConnecting(); InitDisconnecting(); break; // Cancel connecting event
						case BANDSTATE_DISCONNECTING:	break; // No events
						case BANDSTATE_LOWPOWER:		CloseLowPower(); ForceBreakAway(); InitSleep(); break; // Turn off event
						case BANDSTATE_SLEEP:			break;
						default: break;
					}
				}
				else; // How did you get an else? there are only 3 timers...
			}
			break;
			case EVENT_COMMAND:
			switch (message.eventInfo[COMMAND_EVENT_INFO_COMMAND]){
				case EVENT_COMMAND_NOTIFY_DISCONNECT:	connected = false; /* Send back ack command */ break; // Independent of band state // Precursor to a disconnect event // This is sent FROM a host TO a guest (the event will occur on the guest)
				case EVENT_COMMAND_DISCONNECT:			if (!connected) EventDisconnecting(); break; // Will ignore command if not notified of disconnect first // This is sent FROM a host TO a guest (the event will occur on the guest)
				case EVENT_COMMAND_ACK:					/* Take guest handle and send imediatly back a disconnect event. Then remove the handle from the guest array and reset the timeout timer. */ break; // This event is received FROM a guest TO a host (the event will occur on the host)
				case EVENT_COMMAND_SET_COUNT:			/* Sets this band's TIMER2 count value to the received count value. */ break;
			}
			break;
		}
	}
}