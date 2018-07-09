/*
 * queue.h
 *
 * Created: 6/14/2018 11:22:35 AM
 *  Author: braddock.bresnahan
 */ 

#ifndef QUEUE_H_
#define QUEUE_H_

/// Includes

#include <stdlib.h>
#include <stdbool.h>

/// Typedefs

typedef unsigned int uint;

/// Specific Macros

// Hard Macros (do not change)

// Soft Macros (feel free to change)
#define QUEUE_CAPACITY_DEFAULT	4 // In bytes

/// Declarations

struct Queue{
	void* data;
	uint typeSize;
	uint size;
	uint capacity;
	uint byteCapacity;
	uint front;
	uint back;
	bool isBusy;
};

// Name: QUE_InitQueue
// Desc: Initializes a new queue with the given type size and stores it in the given queue.
// Input: (struct Queue* const) queue: The container to store the newly created queue.
// Input: (uint)             typeSize: The type size to give the new queue.
void QUE_InitQueue(volatile struct Queue* const queue, uint typeSize);
// Name: QUE_CloseQueue
// Desc: Closes the given queue.
// Input: (struct Queue*) queue: The queue to close.
void QUE_CloseQueue(volatile struct Queue* queue);
// Name: QUE_CloneQueue
// Desc: Makes a clone of the given original queue and stores it in the clone queue.
// Input: (struct Queue*)    original: The queue to clone.
//        (struct Queue* const) clone: The container to store the new clone.
void QUE_CloneQueue(volatile struct Queue* original, volatile struct Queue* const clone);
// Name: QUE_CopyQueue
// Desc: Copies the elements of the original queue into the given copy queue.
// Input: (struct Queue*)   original: The original queue to copy.
//        (struct Queue* const) copy: The queue to copy elements into.
void QUE_CopyQueue(volatile struct Queue* original, volatile struct Queue* const copy);

// Name: QUE_IsEmpty
// Desc: Returns if the given queue has no queued elements.
// Input: (struct Queue*) queue: The queue to check for emptiness.
// Return: (bool) True if the queue is empty, else false. Function will always return a
//                value, even if there is an error; in which case, the value will be true.
inline bool QUE_IsEmpty(volatile struct Queue* queue){
	return queue->size == 0;
}

// Name: QUE_Enqueue
// Desc: Enqueues the given value into the given array.
// Input: (struct Queue*) queue: The queue to enqueue into.
//        (const void*)   value: A pointer to the value to enqueue.
void QUE_Enqueue(volatile struct Queue* queue, const void* value);
// Name: QUE_Dequeue
// Desc: Dequeues the next value from the given queue and stores the value at the given
//       value pointer.
// Input: (struct Queue*) queue: The queue to dequeue from.
//        (void*)         value: Where to store the dequeued value. If NULL, the value
//                               will not be stored.
// Return: (bool) True if successfully dequeued, else false.
bool QUE_Dequeue(volatile struct Queue* queue, void* value);

// Name: QUE_Clear
// Desc: Clears the given queue of all it's queued elements.
// Input: (struct Queue*) queue: The queue to clear.
void QUE_Clear(volatile struct Queue* queue);


#endif /* QUEUE_H_ */
