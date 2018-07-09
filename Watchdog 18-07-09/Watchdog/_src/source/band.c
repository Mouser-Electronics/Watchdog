/*
 * band.c
 *
 * Created: 6/27/2018 12:08:49 PM
 *  Author: braddock.bresnahan
 */ 

#include "band.h"
#include "atmel_start.h"
#include "messages.h"
#include "interrupts.h"
#include "i2c.h"
#include "boardfuncs.h"
#include "timers.h"

#include "power.h"

/// Function Definitions

void BandMain(){
	// Will only end after all interrupts are idle and there are no events to handle.
	// Unfortunately, all of this is necessary. Otherwise weird things start happening with events and interrupts (ie. double events for a single interrupt).
	/*
	///// THE IDEA /////
	// First checks if it should skip HandleEvents and HandleUpdates.
	// If it should skip, it jumps to the ending check for events.
	// Else it runs HandleEvents and HandleUpdates; then goes to ending check for events.
	// If there are events queued, then it jumps back to the event and update handler; then checks for events again. (Another event might have been queued in that time)
	// Else if there is still an interrupt going, it will jump back to the event check. (The interrupt might have added an event)
	// Else the MCU will go to sleep.
	///// There are a few cases where an event might be missed, but it has the span of only one or two MCU instructions. (Unlikley that it will happen; at least not very often)
	*/
	if (MCUSkip){ // Non important event skips; Skips events and updates (only if there are no events queued)
		MCUSkip = false;
		goto SLEEP;
	}
	/**/BEGIN:/**/
	HandleEvents();
	HandleUpdates();
	/**/SLEEP:/**/
	while (MCUBusy); // Wait for running interrupts to finish
	if (!QUE_IsEmpty(&eventQueue)){
		HandleUpdates();
	}
	if (!QUE_IsEmpty(&eventQueue))
		goto BEGIN; // There are more events to process
	if (MCUBusy)
		goto SLEEP; // In the short amount of time it took to run IsEmpty, an interrupt happened... sigh...
	GoToSleep();
}

void HandleUpdates(){
	switch (bandState){
		case BANDSTATE_DISPLAY:			UpdateDisplay(); break;
		case BANDSTATE_NORMAL:			UpdateNormal(); break;
		case BANDSTATE_CONNECTING:		UpdateConnecting(); break;
		case BANDSTATE_DISCONNECTING:	UpdateDisconnecting(); break;
		case BANDSTATE_LOWPOWER:		UpdateLowPower(); break;
		case BANDSTATE_SLEEP:			UpdateSleep(); break;
		default: break;
	}
}

/// Other

void ForceBreakAway(void){
	// Disconnect from everything immediately with no notifications, updating, initializing or closing anything.
}
	
void FireAlarm(void) { /* Empty */ }
	
/// LED Section

void LEDEnqueue(uint pin, const char* const instructions){
	static struct timer_task task = { .cb = TIMER_CALLBACK, .mode = TIMER_TASK_ONE_SHOT, .interval = 1, };
	static struct QueueInst inst;
	static const char* temp;
	
	inst.misc = pin;
	temp = instructions;
	while (*temp != '\0'){
		switch (*temp){
			case '-':
			case '+':
			case ',':
			case '.':
			case '^':
			case '*': inst.type = INST_LED_ON; break;
			case '_':
			case '=': inst.type = INST_LED_OFF; break;
			default: inst.type = INST_NOP; break;
		}
		QUE_Enqueue(&task4, &inst);
		temp++;
	}
	
	if (!_timer_is_started(&TIMER3.device) && !QUE_IsEmpty(&task4)){
		timer_add_task(&TIMER3, &task);
		timer_start(&TIMER3);
	}
}

void LEDClearQueue(){
	while (!QUE_IsEmpty(&task4)){
		QUE_Dequeue(&task4, NULL);
	}
}

bool LEDIsIdle(){
	return QUE_IsEmpty(&task4) && !_timer_is_started(&TIMER3.device);
}

void LEDDelay(uint milliseconds){
	uint delay = milliseconds / TIMER_LENGTH;
	char* const inst = malloc(delay + 1);
	
	for (uint i = 0; i < delay; i++)
		inst[i] = '$'; // NOP
	LEDEnqueue(0, inst);
	
	free(inst);
}