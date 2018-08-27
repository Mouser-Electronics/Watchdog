/*
 * band.c
 *
 * Created: 6/27/2018 12:08:49 PM
 *  Author: braddock.bresnahan
 */ 

#include "watchdog.h"

/// Function Definitions

void BandInit(){
	MCUSkip = false;
	atmel_start_init();
	SetLED(LEDS, ON);
	
	delay_ms(500); // For aesthetics
	
	InitTimers();
	
	InitPowerModule();
	
	InitEvents();
	
	InitInterrupts();
	
	/// BLE Specific
	SetLED(LEDM, ON);
	InitUSART();
	
	while (!LEDIsIdle()); // Wait for all status indicators to finish
	
	/// Accel Specific
	SetLED(LEDL, ON);
	InitI2C();
	
	while (!LEDIsIdle()); // Wait for all status indicators to finish
	
	/// Other
	isHost = false;
	connected = false;
	bandRange = STARTING_BANDRANGE;
	button = false;
	alarmed = false;
	bondNum = 0;
	led = LEDW;
	
	// Start
	CommandBLE(BLE_CMD_ENTER_COMMAND_MODE, NULL);
	EnableTimer(&TIMER0);
	InitDisplay();
	
	// Successful Init Indicator
	MotorEnqueue("_-_-_");
}

void BandMain(){
	///// THE IDEA /////
	// First checks if it should skip HandleEvents and HandleUpdates.
	// If it should skip, it jumps to the ending check for events.
	// Else it runs HandleEvents and HandleUpdates; then goes to ending check for events.
	// If there are events queued, then it jumps back to the event and update handler; then checks for events again. (Another event might have been queued in that time)
	// Else the MCU will go to sleep.
	///// There are a few cases where an event might be missed, but it has the span of only one or two MCU instructions. (Unlikley that it will happen; at least not often)
	if (MCUSkip) goto SLEEP; // The wakeup was triggered by a non-important event
	/**/BEGIN:/**/
	HandleEvents();
	HandleUpdates();
	MCUSkip = true;
	/**/SLEEP:/**/
	if (!QUE_IsEmpty(writeEventQueue)) // Are there events to process?
		goto BEGIN; // Go and process them
	GoToSleep();
}

// HandleEvents defined in messages.h/messages.c

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
	// Not functional at the moment
	FireAlarm();
}
	
void FireAlarm(void){
	static uint ALARM_SIZE = 0; // Pseudo constant
	uint timeout;
	
	if (ALARM_SIZE == 0) // Pseudo constant (checking 0 is faster than getting a string size)
		ALARM_SIZE = STR_GetSize(ALARM_STRING);
	
	alarmed = true;
	timeout = 0;
	do	{
		if (task3.size <= ALARM_SIZE){
			MotorEnqueue(ALARM_STRING);
			LEDEnqueue(LEDALL, ALARM_STRING);
		}
		delay_ms(1);
		if (timeout++ >= ALARM_TIMEOUT)
			alarmed = false; // Acts as break
	} while (alarmed); // Alarm turned off by timeout or button press
	// Reset motor and LEDs
	MotorClearQueue();
	LEDClearQueue();
	SetMotor(OFF);
	SetLED(LEDALL, OFF);
}