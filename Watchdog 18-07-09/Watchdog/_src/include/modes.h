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

#define LOWPOWER_LED_PIN	LEDR

/// Enums

typedef enum{
	BANDSTATE_NONE = 0,
	BANDSTATE_DISPLAY,
	BANDSTATE_NORMAL,
	BANDSTATE_CONNECTING,
	BANDSTATE_DISCONNECTING,
	BANDSTATE_LOWPOWER,
	BANDSTATE_SLEEP,
} BandState;

/// Function Declarations

// In general, Update functions happen on main clock cycle, Event functions happens on an event.

/* Glossary */
// CALLING MODES - Modes that can initialize the current mode.
//                 The current mode "calls" the mode listed.
//                 The current mode might not literally call the mode.
// FALLING MODES - Modes that can be initialized by the current mode.
//                 The current mode "falls back into" the mode listed.

/// Display Mode
// Shows the current states of the band through LED indicators.
// Shows range setting and band host/guest/free state.
// Does not show low power red led (low power mode does that).
/* CALLING MODES */
// Normal Mode: On button down.
/* FALLING MODES */
// Normal Mode:        On Display Mode timeout.
// Connecting Mode:    On long button press if not connected.
// Disconnecting Mode: On long button press if connected and is host.

/*
// Name: InitDisplay
// Desc: Switches to Display Mode.
*/
void InitDisplay(void);
/*
// Name: UpdateDisplay
// Desc: Updates the LED indicator states and checks button to prolong display mode.
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
// Handles all functions of the band (bluetooth, alarm, etc...)
/* CALLING MODES */
// Display Mode:       On Display Mode timeout.
// Connecting Mode:    On connection timeout whether successful or not.
// Disconnecting Mode: On disconnecting timeout whether successful or not.
// Sleep Mode:         On wakeup event.
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
// Name: Handles bluetooth and vibration motor for alarm.
*/
void UpdateNormal(void);
/*
// Name: EventNormal
// Desc: Fired on motion event. Keeps the band awake. Called directly from motion interrupt.
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
// Sets connection, isHost, host and guests variables. (volatile until completion)
// Uses UpdateDisplay function to display band state.
/* CALLING MODES */
// Display Mode: On long button press when not connected.
/* FALLING MODES */
// Normal Mode:        On timeout.
// Disconnecting Mode: (Conditional) When the counter timeout occurs. (In connection mode for way way too long)

/*
// Name: InitConnecting
// Desc: Switches to Connecting Mode.
*/
void InitConnecting(void);
/*
// Name: UpdateConnecting
*/
// Desc: Handles the entire connection process from start to finish.
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
// Display Mode:    On long button press when host band.
// Connecting Mode: (Conditional) When the counter timer runs out for connecting mode.
/* FALLING MODES */
// Normal Mode: After disconnection or timeout.

/*
// Name: InitDisconnect
// Desc: Switches to Disconnecting Mode.
*/
void InitDisconnecting(void);
/*
// Name: UpdateDisconnecting
// Desc: The host process of a disconnect.
*/
void UpdateDisconnecting(void);
/*
// Name: EventDisconnect
// Desc: The guest process of a disconnect. This function is the one that disconnects the devices.
*/
void EventDisconnecting(void);
/*
// Name: CloseDisconnect
// Desc: Disconnect Mode close.
*/
void CloseDisconnecting(void);

/// Low Power Mode
// If the band is low on power, it will be handled differently.
// If not connected, the band will notify the user and fall asleep.
// If connected, the band will wait to sleep, but will blink red and notify user.
// Will forcibly sleep on brown out, triggering the normal alarm.
// Will go to sleep as soon as disconnected.
// Mode timeout drastically shorter than normal.
// Think of this as a quick Normal Mode.
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
// Puts MCU, bluetooth, clocks and interrupts to sleep except for accelerometer interrupt.
// Accelerometer motion event ends Sleep Mode.
/* CALLING MODES */
// Normal Mode: On Normal Mode timeout.
// Low Power Mode: When disconnected or close to brownout.
/* FALLING MODES */
// Display Mode: On wakeup event. (Falls back to Normal Mode)

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
// Desc: Wake up event handler. Called from accelerometer interrupt.
*/
void EventSleep(void);
/*
// Name: CloseSleep
// Desc: Sleep Mode close. Called on wakeup event.
*/
void CloseSleep(void);

/// Global Variables

/*
// Name: bandState
// Desc: The current main band state of the band.
*/
BandState bandState;

// Mode specific shared variables
static volatile uint displaySVAR;
static volatile uint normalSVAR;
static volatile uint connectingSVAR;
static volatile uint disconnectingSVAR;
static volatile uint lowpowerSVAR;
static volatile uint sleepSVAR;


#endif /* MODES_H_ */