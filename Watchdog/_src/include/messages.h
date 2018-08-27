/*
 * messages.h
 *
 * Created: 6/28/2018 7:56:37 AM
 *  Author: braddock.bresnahan
 */ 

#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "queue.h"
#include "stdtypes.h"

/// Macros

#define MAX_EVENT_INFO_SIZE		16 // The size of the eventInfo of an event message in bytes.

#define BUTTON_EVENT_INFO_BUTTON_STATE	0 // The index of the button state in the eventInfo of a button event message
#define TIMER_EVENT_INFO_SENDER			0 // The index of the timer that caused the event in the eventInfo of a timer event message.

// ONLY AN EXPLANATION, HAS NO FUNCTIONALITY
#define EXPLANATION_INTERRUPT_OVERSHADOW
/*
// This is not a bad on the C21N or the M0+; it could be entierly my fault, but I was
// getting some really weird behavior with interrupts and the event queue. I think it
// had something to do with how an interrupt "takes over" what is currently happening in
// code. If the main loop is running and an interrupt occurs, the main loop will stop
// everything it's doing, regardless of what it is, and handle the interrupt. This can
// cause problems when you're trying to write to a queue while in the middle of a read
// function. There might be a way to fix this 'correctly', I just don't know it yet.
*/

/// Enums

/*
// Name: EventType
// Desc: The subtypes of an event message.
*/
typedef enum{
	EVENT_BUTTON,		// A button event
	EVENT_TIMER,		// A timer event
	EVENT_DISCONNECT,	// A disconnect event
	EVENT_ACCEL,		// An Accelerometer event
} EventType;

/// Structures

/*
// Name: EventMessage
// Desc: A structure that holds the information of an interrupt or polled event.
// Members: (EventType)  eventType: The main category of the event.
//          (uint8*)     eventInfo: Aditional information about the event. Changes
//                                  depending on the event type. Has the total capacity
//                                  of MAX_EVENT_INFO_SIZE.
*/
struct EventMessage{
	EventType type;
	uint8 info[MAX_EVENT_INFO_SIZE];
};

/*
/// EventInfo Glossary
// [index:bytes]

// Button Event Info
// [0:1] (bool) Button state

// Timer Event Info
// [0:4] (tcc_module*) The sender that fired the event

// Disconnection Event info
// None

// Accelerometer Event Info
// None

*/

/// Function Declarations

/*
// Name: InitEvents
// Desc: Initializes the event handling of the band. Call this function as soon as
//       possible from startup.
*/
void InitEvents(void);

/*
// Name: HandleEvents
// Desc: The function that handles all of the events in the band. Called from BandMain.
*/
void HandleEvents(void);

/// Global variables

/*
// Name: readEventQueue
// Desc: The "read from" event queue. Switches between event message queues. Double buffer
//       explained in EXPLANATION_INTERRUPT_OVERSHADOW above. The double buffer didn't fix
//       the problem completely, but it did reduce the recurrence of the bug significantly
//       (once every 40 times to once every 400 times).
*/ 
volatile struct Queue* readEventQueue;
/*
// Name: writeEventQueue
// Desc: The "write to" event queue. Switches between event message queues. Double buffer
//       explained in EXPLANATION_INTERRUPT_OVERSHADOW above. The double buffer didn't fix
//       the problem completely, but it did reduce the recurrence of the bug significantly
//       (once every 40 times to once every 400 times).
*/
volatile struct Queue* writeEventQueue;

/*
// Name: eventMessageQueues
// Desc: The bufferers for the read and write event queues. Double buffer explained in
//       EXPLANATION_INTERRUPT_OVERSHADOW above.
*/
volatile struct Queue eventMessageQueues[2];

/*
// Name: messageQueueInitialized
// Desc: This becomes true when the message system is initialized. Before, the
//       accelerometer could fire before messages could initialize.
*/
volatile static bool messageQueueInitialized = false;


#endif /* MESSAGES_H_ */