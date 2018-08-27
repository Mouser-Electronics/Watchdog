/*
 * modes.c
 *
 * Created: 6/27/2018 11:45:59 AM
 *  Author: braddock.bresnahan
 */ 

#include "watchdog.h"

/// Function Definitions

void InitDisplay(){
	bandState = BANDSTATE_DISPLAY;
	
	SetLED(LEDALL, OFF);
	ConfigTimer(&TIMER1, TIMER_CONFIG_DISPLAY); // Display timeout
	ConfigTimer(&TIMER2, TIMER_CONFIG_DISPLAY_BUTTON); // Long hold event
	BeginTimer(&TIMER1);
	// Timer 2 starts on its own in Event
	SetLED(led, ON);
	UpdateDisplay();
	// No vibration
	displaySVAR = 1; // Skip first up event
}

void UpdateDisplay(){
	uint8 pled;
	if (button) ResetTimer(&TIMER1);
	
	// LED set
	switch(bandRange){ // Update LEDS
		case BANDRANGE_NONE:	SetLED(LEDRNG, OFF); break;
		case BANDRANGE_LONG:	SetLED(LEDL, ON);
		case BANDRANGE_MEDIUM:	SetLED(LEDM, ON);
		case BANDRANGE_SHORT:	SetLED(LEDS, ON); break;
	}
	// Primary LED
	pled = led;
	if (HostBand) // Set new one (HOST) A host will always be connected to something. When the last band disconnects, the band will become free
		led = LEDG;
	else if (GuestBand) // Or set new one (GUEST)
		led = LEDB;
	else // Or set new one (FREE)
		led = LEDW;
	if (pled != led){ // Only change if there is a change
		SetLED(pled, OFF);
		SetLED(led, ON);
	}
}

void EventDisplay(){
	if (button){ // Long press timer start
		BeginTimer(&TIMER2);
		ResetTimer(&TIMER1);
	}
	else{
		if (displaySVAR){ // Ignores first up event
			displaySVAR = 0;
			return;
		}
		
		EndTimer(&TIMER2);
		
		if (GuestBand) return; // Guest bands cannot change range settings
		// Change Range
		switch(bandRange){
			case BANDRANGE_NONE:	bandRange = BANDRANGE_SHORT;	break;
			case BANDRANGE_SHORT:	bandRange = BANDRANGE_MEDIUM;	break;
			case BANDRANGE_MEDIUM:	bandRange = BANDRANGE_LONG;		break;
			case BANDRANGE_LONG:	bandRange = BANDRANGE_NONE;		break;
		}
	}
}

void CloseDisplay(){
	EndTimer(&TIMER1);
	EndTimer(&TIMER2);
	displaySVAR = 0; // Reset initVar
	SetLED(LEDALL, OFF);
}

void InitNormal(){
	bandState = BANDSTATE_NORMAL;
	
	ConfigTimer(&TIMER1, TIMER_CONFIG_NORMAL); // Band alarm trigger
	ConfigTimer(&TIMER2, TIMER_CONFIG_NORMAL_TIMEOUT); // Inactive timeout (sleepy time trigger)
	if (!FreeBand) BeginTimer(&TIMER1);
	BeginTimer(&TIMER2);
	normalSVAR = 0; // Band checking spacer
}

void UpdateNormal(){
	static uint8 bond = 0;
	static uint8 cycle = 0;
	static uint8 hostRange;
	static volatile uint8 guestStrength;
	static char buff[USART_STANDARD_BUFFER_SIZE];
	
	if (FreeBand) return;
	// Only connected bands
	
	if (bondNum < 3 && (cycle++ % 3)) return; // Lengthens check time for only a few bands (saves power).
	
	switch (bandRange){ // Get Range
		case BANDRANGE_NONE:	ResetTimer(&TIMER1); /*Band function off*/	return;
		case BANDRANGE_SHORT:	hostRange = BANDRANGE_SIGNAL_MAX_SHORT;		break;
		case BANDRANGE_MEDIUM:	hostRange = BANDRANGE_SIGNAL_MAX_MEDIUM;	break;
		case BANDRANGE_LONG:	hostRange = BANDRANGE_SIGNAL_MAX_LONG;		break;
	}
	
	if (!normalSVAR){ // Connect to next band
		STR_ClearString(buff);
		STR_NumToHex8(buff, (bond++ % bondNum) + 1); // Decides which guest to connect to
		CommandBLE(BLE_CMD_DISCONNECT_ACTIVE_LINK, NULL); // Make sure there is no active BLE connection
		CommandBLE(BLE_CMD_CONNECT_TO_STORED_DEVICE, (buff + 1)); // (buf + 1) = "00" to "0"
		normalSVAR = true; // Only try to connect to one band at a time
	}
	// Check if connected (every time)
	STR_ClearString(buff); // Debugging
	FlushUSART();
	if (CommandBLEReturn(BLE_CMD_GET_CONNECTION_STATUS, buff)){
		if (!STR_Contains(buff, "none")){ // Is connected
			STR_ClearString(buff);
			/*Might not need this line*/CommandBLE(BLE_CMD_GET_PEER_DEVICE_NAME, buff); // Forces communication so signal strength will update.
			CommandBLEReturn(BLE_CMD_GET_SIGNAL_STRENGTH_OF_LAST_COMMUNICATION, buff);
			guestStrength = STR_HexToInt8(buff + 1);
			if (hostRange >= guestStrength){ // In bounds
				ResetTimer(&TIMER1);
				CommandBLE(BLE_CMD_DISCONNECT_ACTIVE_LINK, NULL);
				normalSVAR = false; // Do next band
			}
		}
	}
	
	/* check for low battery state and change mode if necessary. Will have to write an event message for this. */
}

void EventNormal(){
	// Accelerometer event
	if (bandState == BANDSTATE_NORMAL) // I don't remember why I put this here, but I'm too afraid to remove it.
		ResetTimer(&TIMER2); // Normal mode inactive timer
}

void CloseNormal() {
	EndTimer(&TIMER1);
	EndTimer(&TIMER2);
	CommandBLE(BLE_CMD_DISCONNECT_ACTIVE_LINK, NULL);
	normalSVAR = false;
}

void InitConnecting() {
	if (GuestBand) { FireAlarm(); InitNormal(); return; } // Cannot change group, and should teach them not to do it again.
	bandState = BANDSTATE_CONNECTING;
	
	ConfigTimer(&TIMER2, TIMER_CONFIG_CONNECTING_DURATION); // Connecting timeout
	ConfigTimer(&TIMER1, TIMER_CONFIG_CONNECTING_COUNTER); // Connecting counter
	
	MotorEnqueue(MOTOR_CONNECTING_INIT_ACTION);
	SetLED(LEDALL, OFF);
	
	CommandBLE(BLE_CMD_CANCEL_CONNECTION_ATTEMPT, NULL);
	CommandBLE(BLE_CMD_DISCONNECT_ACTIVE_LINK, NULL);
	CommandBLE(BLE_CMD_CONNECT_TO_PUBLIC_ADDRESS, BLE_PARTNER_ADDRESS);
	/* For scanning for other bands
	//CommandBLE(BLE_CMD_START_ADVERTISING, NULL);
	//CommandBLE(BLE_CMD_START_SCANNING, NULL);
	*/
	
	BeginTimer(&TIMER1);
	BeginTimer(&TIMER2);
	
	isHost = true; // All bands start as host
	connectingSVAR = 0;
}
	
void UpdateConnecting(){
	static char buff[USART_STANDARD_BUFFER_SIZE];
	
	if (button){ // Extend connection timer if the user pressed the button.
		ResetTimer(&TIMER2);
	}	
	
	// LED display connecting
	LEDBlink(led, LED_GENERAL_BLINK_LENGTH);
	
	// For this build, we only need to look for one other, specific band.
	if (!connected){
		STR_ClearString(buff);
		FlushUSART();
		if (CommandBLEReturn(BLE_CMD_GET_CONNECTION_STATUS, buff) == USART_SUCCESS){
			if (!STR_Contains(buff, "none") || STR_Contains(buff, "%C")){
				CommandBLE(BLE_CMD_BOND_CONNECTED_DEVICE, NULL);
				if ((ReadUSART(buff) == USART_SUCCESS) && STR_Contains(buff, "%S")){
					if ((ReadUSART(buff) == USART_SUCCESS) && STR_Contains(buff, "%B")){
						bondNum++;
						connected = true;
						isHost = true;
						led = LEDG;
						LEDEnqueue(led, "++==");
						ResetTimer(&TIMER2);
						CommandBLE(BLE_CMD_DISCONNECT_ACTIVE_LINK, NULL);
						FlushUSART();
					}
				}
			}
		}
	}
	
	
	/* Possible Process For Band Scanning:
		If device is discovered, check to see if it is a band.
		If it is, continue; If not, break;
		If I am a guest and connection is a host, break.
		If I am a host and connection is a guest, check if connection is already in group.
			If it is in the group, break.
			If it is not in the group, Add to group and break.
		If I am a host and connection is a host, check counter timer for who started first.
			If I started first, add connection to connection list.
			If connection started first, become a guest band and break.
		Any successful group connection should reset the connection timeout on all group bands.
	*/
}
	
void EventConnecting() { /* Empty */ }
	
void CloseConnecting() {
	EndTimer(&TIMER1);
	EndTimer(&TIMER2);
	LEDClearQueue();
	SetLED(led, OFF);
	MotorEnqueue(MOTOR_CONNECTING_CLOSE_ACTION);
	/* For scanning for other bands
	//CommandBLE(BLE_CMD_STOP_ADVERTISING, NULL);
	//CommandBLE(BLE_CMD_STOP_SCANNING, NULL);
	*/
}
	
void InitDisconnecting() {
	if (!HostBand) { FireAlarm(); InitNormal(); return; } // Only host bands can disconnect a group
	bandState = BANDSTATE_DISCONNECTING;
	
	ConfigTimer(&TIMER1, TIMER_CONFIG_DISCONNECTING_TIMEOUT);
	
	MotorEnqueue(MOTOR_DISCONNECTING_INIT_ACTION);
	SetLED(LEDALL, OFF);
	
	BeginTimer(&TIMER1);
	disconnectingSVAR = 0;
}
	
void UpdateDisconnecting(){
	static const struct EventMessage message = { EVENT_DISCONNECT };
	
	// Process
	FlushUSART();
	CommandBLE(BLE_CMD_REMOVE_EXISTING_BONDING, ",Z"); // Removes all bonding
	bondNum = 0;
	connected = false;
	isHost = false;
	led = LEDW;
	QUE_Enqueue(writeEventQueue, &message); // Enqueue disconnect event
	
	// LED display disconnecting
	LEDBlink(led, LED_GENERAL_BLINK_LENGTH);
}

void EventDisconnecting() { /* Empty */ }
	
void CloseDisconnecting(){
	EndTimer(&TIMER1);
	MotorEnqueue(MOTOR_DISCONNECTING_CLOSE_ACTION);
	while (!LEDIsIdle()); // Wait for disconnect blinking
}
	
void InitLowPower() {
	bandState = BANDSTATE_LOWPOWER;
	ConfigTimer(&TIMER1, TIMER_CONFIG_LOWPOWER); // Low power timeout
	ConfigTimer(&TIMER2, TIMER_CONFIG_LOWPOWER_BUTTON); // Turn off event
	BeginTimer(&TIMER1);
	MotorEnqueue(MOTOR_LOWPOWER_INIT_ACTION);
	lowpowerSVAR = 0;
}

void UpdateLowPower(){
	static uint task4RequiredLength;
	static const int task4BlinkDuration = 500;
	
	if (!connected){ // As soon as not connected, go to sleep.
		CloseLowPower();
		InitSleep();
	}
	
	// LED display lowpower
	task4RequiredLength = GetDurationOfTimer(&TIMER0)->length + 1;
	while (task4.size <= task4RequiredLength){
		LEDEnqueue(LOWPOWER_LED_PIN, "+");
		LEDDelay(task4BlinkDuration);
		LEDEnqueue(LOWPOWER_LED_PIN, "=");
		LEDDelay(task4BlinkDuration);
	}
	
	/* Check if almost dead, if so kill
	   CloseLowPower
	   ForceBreakAway 
	   bandState = BANDSTATE_NONE // Stops the band from functioning until power cycle */
}
	
void EventLowPower(){ // Holding down button can force break away from a group. Needs redesign.
	if (button)
		BeginTimer(&TIMER2);
	else
		EndTimer(&TIMER2);
}
	
void CloseLowPower(){
	EndTimer(&TIMER1);
	EndTimer(&TIMER2);
	SetLED(LOWPOWER_LED_PIN, OFF); // Make sure led is off.	
}
	
void InitSleep() {
	MotorEnqueue(MOTOR_SLEEP_INIT_ACTION);
	DebugOn;
	bandState = BANDSTATE_SLEEP;
	EndTimer(&TIMER0);
	SetPowerMode(POWERMODE_STANDBY);
	/* Put MCU into suspend or sleep mode. Make sure interrupts are still on.
	   Turn off BLE.
	   Put the BMI160 into an even better power saving mode. The accelerometer does not need to be very accurate to wake up the MCU. */
	sleepSVAR = 0;
	while (!MotorIsIdle() || !LEDIsIdle()); // Wait until motor/LED is done
	GoToSleep(); // Deep sleep
}

void UpdateSleep() { /* Empty */ }
	
void EventSleep(){
	// Accelerometer event
	// Wakeup
	CloseSleep();
	InitDisplay();	
}
	
void CloseSleep(){
	DebugOff;
	BeginTimer(&TIMER0);
	SetPowerMode(POWERMODE_IDLE);
	MotorEnqueue(MOTOR_SLEEP_CLOSE_ACTION);
	/* Put the BMI160 into the normal setting (still low power, but a little more accurate) */
	/* Turn back on the BLE. */
}