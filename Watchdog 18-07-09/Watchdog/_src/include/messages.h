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

// The size of the eventInfo of a message.
#define MAX_EVENT_INFO_SIZE		16 // In bytes

// Indexes of event info
#define BUTTON_EVENT_INFO_BUTTON_STATE	0
#define TIMER_EVENT_INFO_TIMER_EVENT	0
#define TIMER_EVENT_INFO_SENDER			1
#define COMMAND_EVENT_INFO_COMMAND		0
#define COMMAND_EVENT_INFO_RECIEVER		1
#define COMMAND_EVENT_INFO_SENDER		5
#define COMMAND_EVENT_INFO_NEW_COUNT	9

/// Enums

typedef enum{
	EVENT_BUTTON,	// A button event occurred
	EVENT_TIMER,	// A timer event occurred
	EVENT_COMMAND,	// A command event occurred
} EventType;

typedef enum{
	EVENT_TIMER_EXPIRED,	// Self explanatory, the event was caused by a timer expiring/ticking
} EventTimerInfo;

typedef enum{
	EVENT_COMMAND_NOTIFY_DISCONNECT,
	EVENT_COMMAND_DISCONNECT,
	EVENT_COMMAND_ACK,
	EVENT_COMMAND_SET_COUNT, // Guest bands set their timers to the host band.
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

volatile struct Queue* readEventQueue;
volatile struct Queue* writeEventQueue;

volatile struct Queue eventMessageQueues[2];



/*
// Name: eventQueue
// Desc: The queue that holds all events until handled by HandleEvents.
*/
volatile struct Queue eventQueue;
volatile struct Queue readQueue;


#endif /* MESSAGES_H_ */