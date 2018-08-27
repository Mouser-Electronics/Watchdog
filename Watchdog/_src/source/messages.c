/*
 * messages.c
 *
 * Created: 6/28/2018 7:57:25 AM
 *  Author: braddock.bresnahan
 */ 

#include "watchdog.h"

/// Function Definitions

void InitEvents(){
	readEventQueue = &eventMessageQueues[0];
	writeEventQueue = &eventMessageQueues[1];
	
	QUE_InitQueue(readEventQueue, sizeof(struct EventMessage));
	QUE_InitQueue(writeEventQueue, sizeof(struct EventMessage));
	
	messageQueueInitialized = true;
}

void HandleEvents(){
	static struct EventMessage message;
	static volatile struct Queue* temp;
	
	// Switch Buffers
	temp = writeEventQueue;
	writeEventQueue = readEventQueue;
	readEventQueue = temp;
	
	// Handle Events
	while (QUE_Dequeue(readEventQueue, &message)){
		switch (message.type){
			case EVENT_BUTTON:
			if (message.info[BUTTON_EVENT_INFO_BUTTON_STATE] == button) break; // Double event (not supposed to happen). Explained more by EXPLANATION_INTERRUPT_OVERSHADOW.
			else button = message.info[BUTTON_EVENT_INFO_BUTTON_STATE]; // Set button
			if (MotorIsIdle())
				MotorEnqueue(MOTOR_GENERIC_BUTTON_ACTION); // Button feedback
			switch (bandState){ // Mode
				case BANDSTATE_DISPLAY:			EventDisplay(); break;
				case BANDSTATE_NORMAL:			CloseNormal(); InitDisplay(); EventDisplay(); break;
				case BANDSTATE_CONNECTING:		break;
				case BANDSTATE_DISCONNECTING:	break;
				case BANDSTATE_LOWPOWER:		EventLowPower(); break;
				case BANDSTATE_SLEEP:			EventSleep(); break; // Should be handled by the accelerometer, but just in case...
				default: break;
			}
			break;
			case EVENT_TIMER: // Timer functions depend on mode
			if (*(struct timer_task**)&message.info[TIMER_EVENT_INFO_SENDER] == &task0){ // MainTimer
				switch (bandState){ // Actions not in update functions
					case BANDSTATE_DISPLAY:			break;
					case BANDSTATE_NORMAL:			break;
					case BANDSTATE_CONNECTING:		break;
					case BANDSTATE_DISCONNECTING:	break;
					case BANDSTATE_LOWPOWER:		break;
					case BANDSTATE_SLEEP:			break;
					default: break;
				}
			}
			else if (*(struct timer_task**)&message.info[TIMER_EVENT_INFO_SENDER] == &task1){ // Standard Mode timeout (usually)
				switch (bandState){
					case BANDSTATE_DISPLAY:			CloseDisplay();	InitNormal(); break;
					case BANDSTATE_NORMAL:			CloseNormal(); FireAlarm(); InitDisplay(); break;
					case BANDSTATE_CONNECTING:		CloseConnecting(); InitDisplay(); break;
					case BANDSTATE_DISCONNECTING:	CloseDisconnecting(); ForceBreakAway(); InitDisplay(); break; // Disconnect failed
					case BANDSTATE_LOWPOWER:		break;
					case BANDSTATE_SLEEP:			break;
					default: break;
				}
			}
			else if (*(struct timer_task**)&message.info[TIMER_EVENT_INFO_SENDER] == &task2){ // Special Mode timeout (usually)
				switch (bandState){
					case BANDSTATE_DISPLAY:			CloseDisplay(); if (connected){ InitDisconnecting(); } else { InitConnecting(); } break; // button held event
					case BANDSTATE_NORMAL:			CloseNormal(); InitSleep(); break; // Inactive timeout event
					case BANDSTATE_CONNECTING:		CloseConnecting(); InitDisplay(); break; // Cancel connecting event
					case BANDSTATE_DISCONNECTING:	break;
					case BANDSTATE_LOWPOWER:		CloseLowPower(); ForceBreakAway(); InitSleep(); break; // Something went wrong in disconnecting
					case BANDSTATE_SLEEP:			break;
					default: break;
				}
			}
			else;
			break;
			case EVENT_DISCONNECT:
				CloseDisconnecting();
				InitDisplay();
			break;
			case EVENT_ACCEL:
			switch (bandState){ // Mode
				case BANDSTATE_DISPLAY:			break;
				case BANDSTATE_NORMAL:			EventNormal(); break; // Stay awake
				case BANDSTATE_CONNECTING:		break;
				case BANDSTATE_DISCONNECTING:	break;
				case BANDSTATE_LOWPOWER:		break;
				case BANDSTATE_SLEEP:			EventSleep(); break; // Wakeup
				default: break;
			}
			break;
			default: break;
		}
	}
}