/*
 * band.h
 *
 * Created: 6/27/2018 12:04:37 PM
 *  Author: braddock.bresnahan
 */ 

#ifndef BAND_H_
#define BAND_H_

#include <stdbool.h>
#include "queue.h"
#include "array.h"
#include "stdtypes.h"
#include "modes.h"
#include "timers.h"

/// Macros

#define STARTING_BANDRANGE		BANDRANGE_SHORT // The default starting band range.
/// The lower the signal strength minimum, the shorter the range
#define BANDRANGE_SIGNAL_MAX_SHORT	70  // The maximum RSSI signal strength for short range
#define BANDRANGE_SIGNAL_MAX_MEDIUM	90  // The maximum RSSI signal strength for medium range
#define BANDRANGE_SIGNAL_MAX_LONG	110 // The maximum RSSI signal strength for long range

#define MAX_BONDED_BANDS	8 // Limited by hardware

#define ALARM_TIMEOUT	17000 // In milliseconds
#define ALARM_STRING	"_^^^__^^__^^^^^^^_^^^_^^__^^__^^^__^^^^^^^_^^^_^^_" // Get their attention

#define HostBand	((volatile bool)(connected && isHost))
#define GuestBand	((volatile bool)(connected && !isHost))
#define FreeBand	((volatile bool)(!connected))

/// Enums

/*
// Name: BandRange
// Desc: The possible Bluetooth ranges for the band.
*/
typedef enum{
	BANDRANGE_NONE,		// 0 meters
	BANDRANGE_SHORT,	// About 10 meters
	BANDRANGE_MEDIUM,	// About 30~40 meters
	BANDRANGE_LONG,		// About 60~80 meters
} BandRange;

/// Function Declarations

/*
// Name: BandInit
// Desc: Initializes the board, modules and all things in the project.
*/
void BandInit(void);
/*
// Name: BandMain
// Desc: The main function of the MCU. Handles events and updates. Puts the MCU to sleep
//       after completion.
*/
void BandMain(void);
/*
// Name: HandleUpdates
// Desc: Updates the band using the update function of the current band mode.
*/
void HandleUpdates(void);

/*
// Name: ForceBreakAwak
// Desc: Disconnects the band from everything immediately with no notifications, updating,
//       initializing or closing anything.
*/
void ForceBreakAway(void);
/*
// Name: FireAlarm
// Desc: Not a fire alarm. Fires the alarm for the band. Does not return until alarm is
//       done.
*/
void FireAlarm(void);

/// Global Variables

/*
// Name: MCUSkip
// Desc: A variable that is set to true when the current interrupt event is not important
//       enough to run an entire event and update handling. Used in BandMain to skip over
//       the handling functions. The variable is overridden by a timer interrupt and
//       ignored if there are events queued to be handled.
*/
volatile bool MCUSkip;

/*
// Name: guests
// Desc: An array holding all connected guest bands.
*/
volatile struct Array guests; // Unused
/*
// Name: isHost
// Desc: Whether this band is a guest or the host of a group.
*/
volatile bool isHost;
/*
// Name: connected
// Desc: Whether this band is connected to a group.
*/
volatile bool connected;
/*
// Name: bandRange
// Desc: The current Bluetooth range of the band.
*/
volatile BandRange bandRange;
/*
// Name: button
// Desc: The current state of the button. True if pressed and false is not pressed.
*/
volatile bool button;
/*
// Name: alarmed
// Desc: The current state of the alarm. True if alarm active, else false.
*/
volatile bool alarmed;

#endif /* BAND_H_ */