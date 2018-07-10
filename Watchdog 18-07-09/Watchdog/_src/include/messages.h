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
#define TIMER_EVENT_INFO_TIMER_EVENT	0 // The index of the type of timer event in the eventInfo of a timer event message
#define TIMER_EVENT_INFO_SENDER			1 // The index of the timer that caused the event in the eventInfo of a timer event message.
#define COMMAND_EVENT_INFO_COMMAND		0 // The index of the specific command in the eventInfo of a command event message.
#define COMMAND_EVENT_INFO_RECIEVER		1 // The index of the target of the command in the eventInfo of a command event message.
#define COMMAND_EVENT_INFO_SENDER		5 // The index of the sender of the command in the eventInfo of a command event message.
#define COMMAND_EVENT_INFO_NEW_COUNT	9 // The index of the count to set as the receiver's timer in the eventInfo of a SET_COUNT command event message.

// NO FUNCTIONALITY (Explanation in comments below)
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
	EVENT_BUTTON,	// A button event occurred
	EVENT_TIMER,	// A timer event occurred
	EVENT_COMMAND,	// A command event occurred
} EventType;

/*
// Name: EventTimerInfo
// Desc: Virtually useless, but it would take time to remove from the code; so here it stays.
*/
typedef enum{
	EVENT_TIMER_EXPIRED,	// Self explanatory, the event was caused by a timer expiring/ticking
} EventTimerInfo;

// Name: EventCommandInfo
// Desc: The types of commands the command event can have.
typedef enum{
	EVENT_COMMAND_NOTIFY_DISCONNECT,	// Notifies the receiving guest band to prepare to disconnect.
	EVENT_COMMAND_DISCONNECT,			// Commands the receiving guest band to disconnect.
	EVENT_COMMAND_ACK,					// Notifies the receiving band of a command acknowledgment.
	EVENT_COMMAND_SET_COUNT,			// Commands the receiving band to set timer count to the count in the eventInfo.
} EventCommandInfo;

/// Structures

/*
// Name: EventMessage
// Desc: A structure that holds the information of an interrupt or polled event.
// Members: (EventType)  eventType: The main category of the event.
//          (uint8) eventBandState: the band state of when the event occurred.
//          (uint8*)     eventInfo: Aditional information about the event. Changes
//                                  depending on the event type. Has the total capacity
//                                  of MAX_EVENT_INFO_SIZE.
*/
struct EventMessage{
	EventType eventType;
	uint8 eventBandState;
	uint8 eventInfo[MAX_EVENT_INFO_SIZE];
};

/*
/// EventInfo Glossary

// Button Event Info
// [0:1] (bool) Button state

// Timer Event Info
// [0:1] (EventTimerInfo) Timer event
// [1:4] (tcc_module*) The sender that fired the event

// Command Event Info
// [0:1] Command
// [1:4] Receiver handle
// [5:4] Sender handle
// (SET_COUNT) [9:4] New count value

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
//       explained by EXPLANATION_INTERRUPT_OVERSHADOW above. The double buffer didn't fix
//       the problem completely, but it did reduce the recurrence of the bug significantly
//       (once every 40 times to once every 400 times).
*/ 
volatile struct Queue* readEventQueue;
/*
// Name: writeEventQueue
// Desc: The "write to" event queue. Switches between event message queues. Double buffer
//       explained by EXPLANATION_INTERRUPT_OVERSHADOW above. The double buffer didn't fix
//       the problem completely, but it did reduce the recurrence of the bug significantly
//       (once every 40 times to once every 400 times).
*/
volatile struct Queue* writeEventQueue;

/*
// Name: eventMessageQueues
// Desc: The bufferers for the read and write event queues. Double buffer explained by
//       EXPLANATION_INTERRUPT_OVERSHADOW above.
*/
volatile struct Queue eventMessageQueues[2];


#endif /* MESSAGES_H_ */