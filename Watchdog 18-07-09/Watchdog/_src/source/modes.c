/*
 * modes.c
 *
 * Created: 6/27/2018 11:45:59 AM
 *  Author: braddock.bresnahan
 */ 

#include "modes.h"
#include "driver_init.h"
#include "band.h"
#include "boardfuncs.h"
#include "timers.h"
#include "power.h"
#include "motor.h"

/// Function Definitions

void InitDisplay(){
	bandState = BANDSTATE_DISPLAY;
	ConfigTimer(&TIMER1, TIMER_CONFIG_DISPLAY); // Display timeout
	ConfigTimer(&TIMER2, TIMER_CONFIG_DISPLAY_BUTTON); // Long hold event
	EnableTimer(&TIMER1);
	EnableTimer(&TIMER2); // We know the button has been pressed from handle events
	SetLED(led, ON);
	UpdateDisplay();
	// No vibration
	displaySVAR = 1; // Skip first up event
}

void UpdateDisplay(){
	if (button) ResetTimerCount(&TIMER1);
	
	// LED set
	switch(bandRange){ // Update LEDS
		case BANDRANGE_NONE:
			SetLED(SLED, OFF);
			SetLED(MLED, OFF);
			SetLED(LLED, OFF);
		break;
		case BANDRANGE_LONG:
			SetLED(LLED, ON);
		case BANDRANGE_MEDIUM:
			SetLED(MLED, ON);
		case BANDRANGE_SHORT:
			SetLED(SLED, ON);
		break;
	}
	// Primary LED clear and set
	SetLED(led, OFF); // Clear old one
	if (isHost) // Set new one (HOST) A host will always be connected to something. When the last band disconnects, the band will become free
		led = LEDG;
	else if (connected) // Or set new one (GUEST)
		led = LEDB;
	else // Or set new one (FREE)
		led = LEDW;
	SetLED(led, ON); // Update new one
}

void EventDisplay(){
	if (displaySVAR){ // Skip first up event
		displaySVAR = 0;
		return;
	}
	// Change range on button up
	if (!button){
		// Change Range
		switch(bandRange){
			case BANDRANGE_NONE:
			bandRange = BANDRANGE_SHORT;
			break;
			case BANDRANGE_SHORT:
			bandRange = BANDRANGE_MEDIUM;
			break;
			case BANDRANGE_MEDIUM:
			bandRange = BANDRANGE_LONG;
			break;
			case BANDRANGE_LONG:
			bandRange = BANDRANGE_NONE;
			break;
		}
		DisableTimer(&TIMER2); // Button held timer
		//MotorEnqueue(MOTOR_DISPLAY_EVENT_ACTION);
	}
	else{
		// Extend timer
		ResetTimerCount(&TIMER1); // Display timeout timer
		EnableTimer(&TIMER2); // Button held timer
	}
}

void CloseDisplay(){
	DisableTimer(&TIMER1);
	DisableTimer(&TIMER2);
	displaySVAR = 0;
	SetLED(SLED, OFF);
	SetLED(MLED, OFF);
	SetLED(LLED, OFF);
	SetLED(LEDR, OFF);
	SetLED(LEDG, OFF);
	SetLED(LEDB, OFF);
	SetLED(LEDW, OFF);
}

void InitNormal(){
	bandState = BANDSTATE_NORMAL;
	ConfigTimer(&TIMER2, TIMER_CONFIG_NORMAL); // Inactive timeout
	EnableTimer(&TIMER2);
	// No vibration
	normalSVAR = 0;
}

void UpdateNormal(){
	// Host update
	/* Poll Bluetooth devices for updates and react accordingly (Dear Me, Do more research you dumb butt. Sincerely, Me) */
	
	/* Alert Vibration when out of range [ ___----__-_-___----__-_-___----__-_-____ ] times however long it should lase */
	
	/* check for low battery state and change mode if necessary. */
	//if (false) { CloseNormal(); InitLowPower(); }
	
	// Guest update (if it needs it)
	
	// Free update
	// Idle state
}

void EventNormal(){ // Called directly from interrupt (does not go through event queue)
	// Accelerometer event
	ResetTimerCount(&TIMER2); // Normal mode inactive timer
}

void CloseNormal() {
	DisableTimer(&TIMER2);
}

void InitConnecting() {
	bandState = BANDSTATE_CONNECTING;
	ConfigTimer(&TIMER1, TIMER_CONFIG_CONNECTING); // Connecting timeout
	ConfigTimer(&TIMER2, TIMER_CONFIG_CONNECTING_TIMEOUT); // Start time counter
	EnableTimer(&TIMER1);
	EnableTimer(&TIMER2);
	MotorEnqueue(MOTOR_CONNECTING_INIT_ACTION);
	connectingSVAR = 0;
}
	
void UpdateConnecting(){
	static uint task4CheckValue;
	static const int task4BlinkDuration = 500;
	
	// LED display connecting
	task4CheckValue = 1.5 * GetDurationOfTimer(&TIMER0)->length;
	while (task4.size < task4CheckValue){
		LEDEnqueue(led, "+");
		LEDDelay(task4BlinkDuration);
		LEDEnqueue(led, "=");
		LEDDelay(task4BlinkDuration);
	}
	
	//LEDEnqueue(LLED, "+=");
	
	
	/* Might want to do slow, gradual blinking.
	   Check with every potential connection if they are a band.
	   Filter out non bands.
	   If there are no nearby connecting bands, then this is the host. (but might change later)
	   If there are nearby connecting bands bands, then this is a guest. (but might change later)
	   Preset states accordingly.
	   Rare edge case when two bands start connecting at the exact same time, both will end up guest bands. */
	
	/* Needs to set 'host', 'isHost' and 'connected' */
	
	/* Obtain list of all valid hosts.
	   If I am a guest with no valid hosts nearby, become a host. Reset host timeout.
	   If I am a guest, find the host with the earliest start time.
	   If I'm already connected to them, do nothing.
	   If I'm not, connect and reset host timeout.
	   If I am a host, and I see another host, compare count values using TIMER2. the band with the greater count value started first.
	   If I started first, do nothing.
	   If I didn't start first, disconnect with any guests that are connected to the band, change to a guest band and connect to other host.
	   All bands will reset timeout upon a new connection and upon a disconnection.
	   Set all guest band timeouts to that of the host. If disconnecting, set to beginning.
	   Do not run disconnect mode to do this, just to it manually. The bands are close enough that there wont be a mistake.
	   Connection range is determined by the host band. A line of bands will not connect all of them. Only the ones in range of the host will connect.
	   Do not vibrate on these connections, as there might be a lot of them. */
	
	/* Set connection state of this band.
	   Set the host handle
	   All host and guest bands will wait for host timeout. (To correct any host mistakes)(No way to end it sooner) */
}
	
void EventConnecting() { /* Empty */ }
	
void CloseConnecting() {
	LEDClearQueue();
	SetLED(led, OFF);
	DisableTimer(&TIMER1);
	DisableTimer(&TIMER2);
	MotorEnqueue(MOTOR_CONNECTING_CLOSE_ACTION);
}
	
void InitDisconnecting() {
	if (!connected || !isHost) return;
	/* Redo because the concept of this timer is flawed. */
	bandState = BANDSTATE_DISCONNECTING;
	ConfigTimer(&TIMER1, DISCONNECTING_TIMER_TIMEOUT);
	EnableTimer(&TIMER1);
	MotorEnqueue(MOTOR_DISCONNECTING_INIT_ACTION);
	UpdateDisplay();
	disconnectingSVAR = 0;
}
	
void UpdateDisconnecting(){
	static uint task4CheckValue;
	static const uint task4BlinkDuration = 500;
	
	if (!connected || !isHost) return;
	
	// LED display disconnecting
	task4CheckValue = 1.5 * GetDurationOfTimer(&TIMER0)->length;
	while (task4.size < task4CheckValue){
		LEDEnqueue(led, "+");
		LEDDelay(task4BlinkDuration);
		LEDEnqueue(led, "=");
		LEDDelay(task4BlinkDuration);
	}
	// Motor
	MotorEnqueue(MOTOR_DISCONNECTING_UPDATE_ACTION);
	
	UpdateDisplay();
	
	/* Notify all guest bands of the disconnect
	   Notify every time in the mode. In case a guest didn't hear
	   Send EVENT_HOST_NOTIFY_DISCONNECT message to guests
	   Remove any quest that was disconnected from the guest array */
	
	// Actual bluetooth disconnect handled in guest.
	
	// If the guest array is empty, then close disconnecting.
}

void EventDisconnecting(){
	// Guest disconnect
	// Mostly just updating variables.
	
	// Connected variable already set to false (but not disconnected yet)
	
	/* Handle disconnect from host */
	
	connected = false;
	host = NULL;
	/* Band will return back to whatever it was doing before.
	   Update display
	   wait a bit delay_s()
	   Clear display */
	
	LEDEnqueue(led, MOTOR_DISCONNECTING_EVENT_ACTION); // Lines up with the motor queue
	MotorEnqueue(MOTOR_DISCONNECTING_EVENT_ACTION);
	
	// Will go back to whatever it was doing before
}
	
void CloseDisconnecting(){
	if (!connected || !isHost) return;
	connected = false;
	isHost = false;
	DisableTimer(&TIMER1);
	MotorEnqueue(MOTOR_DISCONNECTING_CLOSE_ACTION);
	InitDisplay(); // Shows new color, then will fall back into normal
}
	
void InitLowPower() {
	bandState = BANDSTATE_LOWPOWER;
	ConfigTimer(&TIMER1, TIMER_CONFIG_LOWPOWER); // Low power timeout
	ConfigTimer(&TIMER2, TIMER_CONFIG_LOWPOWER_BUTTON); // Turn off event
	EnableTimer(&TIMER1);
	MotorEnqueue(MOTOR_LOWPOWER_INIT_ACTION);
	lowpowerSVAR = 0;
}

void UpdateLowPower(){
	static uint task4CheckValue;
	static const int task4BlinkDuration = 500;
	
	if (!connected){ // As soon as not connected, go to sleep.
		CloseLowPower();
		InitSleep();
	}
	
	// LED display lowpower
	task4CheckValue = 1.5 * GetDurationOfTimer(&TIMER0)->length;
	while (task4.size <= task4CheckValue){
		LEDEnqueue(LOWPOWER_LED_PIN, "+");
		LEDDelay(task4BlinkDuration);
		LEDEnqueue(LOWPOWER_LED_PIN, "=");
		LEDDelay(task4BlinkDuration);
	}
	
	/* Check if almost dead, if so kill
	   CloseLowPower
	   ForceBreakAway */
}
	
void EventLowPower(){
	if (button)
		EnableTimer(&TIMER2);
	else
		DisableTimer(&TIMER2);
}
	
void CloseLowPower(){
	DisableTimer(&TIMER1);
	DisableTimer(&TIMER2);
	SetLED(LOWPOWER_LED_PIN, OFF); // Make sure led is off.	
}
	
void InitSleep() {
	MotorEnqueue(MOTOR_SLEEP_INIT_ACTION);
	DebugOn;
	bandState = BANDSTATE_SLEEP;
	DisableTimer(&TIMER0);
	SetPowerMode(POWERMODE_STANDBY);
	while (!MotorIsIdle()); // Wait until motor is done
	GoToSleep();
	/* Put MCU into suspend or sleep mode. Make sure interrupts are still on.
	   Put the BMI160 into an even better power saving mode. The accelerometer does not need to be very accurate to wake up the MCU. */
	sleepSVAR = 0;
}

void UpdateSleep() { /* Empty */ }
	
void EventSleep(){ // Called directly from interrupt (does not go through event queue)
	// Accelerometer event
	/* Wake up MCU */
	delay_ms(10); // Delay however long it takes to wake up the MCU
	CloseSleep();
	InitDisplay();	
}
	
void CloseSleep(){
	EnableTimer(&TIMER0);
	SetPowerMode(POWERMODE_IDLE);
	MotorEnqueue(MOTOR_SLEEP_CLOSE_ACTION);
	/* Put the BMI160 into the normal setting (still low power, but a little more accurate) */
}