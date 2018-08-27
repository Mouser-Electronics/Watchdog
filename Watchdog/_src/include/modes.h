/*
 * modes.h
 *
 * Created: 6/27/2018 8:10:45 AM
 *  Author: braddock.bresnahan
 */ 

#ifndef MODES_H_
#define MODES_H_

#include "atmel_start_pins.h"
#include "stdtypes.h"

/// Macros

#define LOWPOWER_LED_PIN	LEDR // The pin to use for low power mode blinking.

/// Enums

/*
// Name: BandState
// Desc: The possible band modes of a band.
*/
typedef enum{
	BANDSTATE_NONE = 0,			// No Mode (band freeze, needs power cycle to start again)
	BANDSTATE_DISPLAY,			// Display Mode
	BANDSTATE_NORMAL,			// Normal Mode
	BANDSTATE_CONNECTING,		// Connecting Mode
	BANDSTATE_DISCONNECTING,	// Disconnecting Mode
	BANDSTATE_LOWPOWER,			// LowPower mode
	BANDSTATE_SLEEP,			// Sleep Mode
} BandState;

/// Function Declarations

// In general, Update functions happen in main loop, Event functions happen on an event (but handled in main loop).

/* Glossary */
// CALLING MODES - Modes that can initialize the current mode.
//                 The mode listed "calls" this mode.
//                 The mode will not directly call this mode (goes through message handling).
// FALLING MODES - Modes that can be initialized by the current mode.
//                 The this mode "falls back into" the mode listed.



/// Display Mode
// Shows the current states of the band through LED indicators.
// Shows range setting and band host/guest/free state.
// Does not show low power red led (low power mode does that).
/* CALLING MODES */
// Normal Mode:        On button down.
// Connecting Mode:    On Mode timeout.
// Disconnecting Mode: On Mode timeout.
// Sleep Mode:         On band wakeup.
/* FALLING MODES */
// Normal Mode:        On Display Mode timeout.
// Connecting Mode:    On long button press if not connected to a group.
// Disconnecting Mode: On long button press if connected to a group.

/*
// Name: InitDisplay
// Desc: Switches to Display Mode.
*/
void InitDisplay(void);
/*
// Name: UpdateDisplay
// Desc: Updates the LED indicator states and checks button to prolong display mode. An
//       extension of Normal Mode.
*/
void UpdateDisplay(void);
/*
// Name: EventDisplay
// Desc: Called on button event when in display mode. Increments the range setting.
*/
void EventDisplay(void);
/*
// Name: CloseDisplay
// Desc: Display Mode close.
*/
void CloseDisplay(void);

/// Normal Mode
// The general state of the band. The band will spend the most time in this state.
// Handles all functions of the band (Bluetooth, alarm, etc...)
/* CALLING MODES */
// Display Mode:       On Display Mode timeout.
/* FALLING MODES */
// Display Mode:   On button down.
// Low Power Mode: When band is low on power.
// Sleep Mode:     On Normal Mode timeout.

/*
// Name: InitNormal
// Desc: Switches to Normal Mode.
*/
void InitNormal(void);
/*
// Name: UpdateNormal
// Name: Handles Bluetooth and vibration motor for alarm.
*/
void UpdateNormal(void);
/*
// Name: EventNormal
// Desc: Fired on motion event. Keeps the band awake.
*/
void EventNormal(void);
/*
// Name: CloseNormal
// Desc: Normal Mode close.
*/
void CloseNormal(void);

/// Connecting Mode
// Connects with other bands.
// Decides host and guest bands with group.
// Sets connection, isHost, and guests variables.
/* CALLING MODES */
// Display Mode: On long button press when not connected.
/* FALLING MODES */
// Display Mode: On timeout or Mode expiration timeout.

/*
// Name: InitConnecting
// Desc: Switches to Connecting Mode.
*/
void InitConnecting(void);
/*
// Name: UpdateConnecting
// Desc: Handles the entire connection process from start to finish.
*/
void UpdateConnecting(void);
/*
// Name: EventConnecting
// Desc: No function.
*/
void EventConnecting(void);
/*
// Name: CloseConnecting
// Desc: Connecting Mode close.
*/
void CloseConnecting(void);

/// Disconnecting Mode
// Disconnects with other bands without setting off the alarm.
// Can only be activated by the host band.
/* CALLING MODES */
// Display Mode:    On long button press and connected.
/* FALLING MODES */
// Display Mode: After disconnection or timeout.

/*
// Name: InitDisconnect
// Desc: Switches to Disconnecting Mode.
*/
void InitDisconnecting(void);
/*
// Name: UpdateDisconnecting
// Desc: Performs the disconnection.
*/
void UpdateDisconnecting(void);
/*
// Name: EventDisconnect
// Desc: No function.
*/
void EventDisconnecting(void);
/*
// Name: CloseDisconnect
// Desc: Disconnect Mode close.
*/
void CloseDisconnecting(void);

/********** NOT FUNCTIONAL IN THIS BUILD **********/
/// Low Power Mode
// If the band is low on power, it will be handled differently.
// If not connected, the band will notify the user and fall asleep.
// If connected, the band will wait to sleep, but will blink red and notify user.
// Will forcibly sleep on brown out, triggering the normal alarm.
// Will go to sleep as soon as disconnected from a group.
/* CALLING MODES */
// Normal Mode: When low on power detected.
/* FALLING MODES */
// Sleep Mode: When disconnected or near brownout.

/*
// Name: InitLowPower
// Desc: Switches to Low Power Mode.
*/
void InitLowPower(void);
/*
// Name: UpdateLowPower
// Desc: Checks for disconnected or brownout and updates the Low Power Mode LED.
*/
void UpdateLowPower(void);
/*
// Name: EventLowPower
// Desc: Long button pressed timer handler.
*/
void EventLowPower(void);
/*
// Name: CloseLowPower
// Desc: Low Power Mode close.
*/
void CloseLowPower(void);

/// Sleep Mode
// Long sleep mode for the band.
// Starts when the band has not moved (no one wearing it) for a long time.
// Puts MCU, into deep sleep.
// Accelerometer motion event ends Sleep Mode.
/* CALLING MODES */
// Normal Mode: On Normal Mode timeout.
// Low Power Mode: When disconnected or near brownout.
/* FALLING MODES */
// Display Mode: On wakeup event.

/*
// Name: InitSleep
// Desc: Switch to Sleep Mode.
*/
void InitSleep(void);
/*
// Name: UpdateSleep
// Desc: No function.
*/
void UpdateSleep(void);
/*
// Name: EventSleep
// Desc: Wake-up function.
*/
void EventSleep(void);
/*
// Name: CloseSleep
// Desc: Sleep Mode close. Called on wake-up event.
*/
void CloseSleep(void);

/// Global Variables

/*
// Name: bandState
// Desc: The band's current band mode.
*/
BandState bandState;

// Mode specific shared variables (Some/Most might never be used).
// A terrible solution for an insignificant problem.

/*
// Name: displaySVAR
// Desc: A variable for sharing information between display mode functions.
*/
static volatile uint displaySVAR;
/*
// Name: normalSVAR
// Desc: A variable for sharing information between normal mode functions.
*/
static volatile uint normalSVAR;
/*
// Name: connectingSVAR
// Desc: A variable for sharing information between connecting mode functions.
*/
static volatile uint connectingSVAR;
/*
// Name: disconnectingSVAR
// Desc: A variable for sharing information between disconnecting mode functions.
*/
static volatile uint disconnectingSVAR;
/*
// Name: lowpowerSVAR
// Desc: A variable for sharing information between low power mode functions.
*/
static volatile uint lowpowerSVAR;
/*
// Name: sleepSVAR
// Desc: A variable for sharing information between sleep mode functions.
*/
static volatile uint sleepSVAR;


#endif /* MODES_H_ */