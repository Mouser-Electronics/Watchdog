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

#define QUEUE_SUCCESS	true
#define QUEUE_FAIL		false

/// Declarations

struct Queue{
	void* data;
	uint typeSize;
	uint size;
	uint capacity;
	uint byteCapacity;
	uint front;
	uint back;
};

// Name: QUE_IsValid
// Desc: Returns whether the given queue is a valid/operational queue.
// Input: (const struct Queue*) queue: The queue to test.
// Return: (bool) True if the queue is valid, else false.
inline bool QUE_IsValid(volatile const struct Queue* queue){
	if (queue != NULL) if (queue->data != NULL) return true;
	return false;
}
// Name: QUE_IsNotValid
// Desc: The same as !QUE_IsValid, but more readable.
// Input: (const struct Queue*) queue: The queue to test.
// Return: (bool) True if the queue is not valid, else false.
inline bool QUE_IsNotValid(volatile const struct Queue* queue){
	return !QUE_IsValid(queue);
}

// Name: QUE_InitQueue
// Desc: Initializes a new queue with the given type size and stores it in the given queue.
// Input: (struct Queue* const) queue: The container to store the newly created queue.
// Input: (uint)             typeSize: The type size to give the new queue.
// Return: (bool) True if the operation was successful, else false.
bool QUE_InitQueue(volatile struct Queue* const queue, uint typeSize);
// Name: QUE_CloseQueue
// Desc: Closes the given queue.
// Input: (struct Queue*) queue: The queue to close.
// Return: (bool) True if the operation was successful, else false.
bool QUE_CloseQueue(volatile struct Queue* queue);
// Name: QUE_CloneQueue
// Desc: Makes a clone of the given original queue and stores it in the clone queue. Think
//       of this as an init and copy.
// Input: (const struct Queue* const) dst: The container to store the new clone.
//        (struct Queue* const)       src: The queue to clone.
// Return: (bool) True if the operation was successful, else false.
bool QUE_CloneQueue(volatile struct Queue* const dst, volatile const struct Queue* const src);
// Name: QUE_CopyQueue
// Desc: Copies the elements of the original queue into the given copy queue. The
//       destination must be an already initialized queue.
// Input: (const struct Queue* const) dst: The queue to copy elements into.
//        (struct Queue*)             src: The original queue to copy.
// Return: (bool) True if the operation was successful, else false.
bool QUE_CopyQueue(volatile struct Queue* const dst, volatile const struct Queue* const src);

// Name: QUE_IsEmpty
// Desc: Returns if the given queue has no queued elements.
// Input: (const struct Queue*) queue: The queue to check for emptiness.
// Return: (bool) True if the queue is empty, else false. Function will always return a
//                value, even if there is an error; in which case, the value will be true.
inline bool QUE_IsEmpty(volatile const struct Queue* queue){
	if (QUE_IsValid(queue)) return queue->size == 0;
	else return true;
}
// Name: QUE_IsNotEmpty
// Desc: Returns if the given queue has queued elements.
// Input: (const struct Queue*) queue: The queue to check for elements.
// Return: (bool) True if the queue is not empty, else false. Function will always return a
//                value, even if there is an error; in which case, the value will be false.
inline bool QUE_IsNotEmpty(volatile const struct Queue* queue){
	if (QUE_IsValid(queue)) return queue->size != 0;
	else return false;
}

// Name: QUE_Enqueue
// Desc: Enqueues the given value into the given array.
// Input: (struct Queue*) queue: The queue to enqueue into.
//        (const void*)   value: A pointer to the value to enqueue.
// Return: (bool) True if the operation was successful, else false.
bool QUE_Enqueue(volatile struct Queue* queue, const void* value);
// Name: QUE_Dequeue
// Desc: Dequeues the next value from the given queue and stores the value at the given
//       value pointer.
// Input: (struct Queue*) queue: The queue to dequeue from.
//        (void*)         value: Where to store the dequeued value. If NULL, the value
//                               will not be stored.
// Return: (bool) True if the operation was successful, else false.
bool QUE_Dequeue(volatile struct Queue* queue, void* value);
// Name: QUE_Peek
// Desc: Peeks at the next value from the given queue and stores the value at the given
//       value pointer.
// Input: (const struct Queue*) queue: The queue to peek at.
//        (void*)               value: Where to store the peeked value.
// Return: (bool) True if the operation was successful, else false.
bool QUE_Peek(volatile const struct Queue* queue, void* value);
// Name: QUE_RememberEnqueue
// Desc: Remembers the last enqueued element in the given queue. WARNING: Unsafe
// Input: (const struct Queue*) queue: The queue to remember from.
//        (void*)               value: Where to store the remembered value.
// Return: (bool) True if the operation was successful, else false.
bool QUE_RememberEnqueue(volatile const struct Queue* queue, void* value);
// Name: QUE_RememberDequeue
// Desc: Remembers the last dequeued element in the given queue. WARNING: Unsafe
// Input: (const struct Queue*) queue: The queue to remember from.
//        (void*)               value: Where to store the remembered value.
// Return: (bool) True if the operation was successful, else false.
bool QUE_RememberDequeue(volatile const struct Queue* queue, void* value);

// Name: QUE_Clear
// Desc: Clears the given queue of all it's queued elements.
// Input: (struct Queue*) queue: The queue to clear.
// Return: (bool) True if the operation was successful, else false.
bool QUE_Clear(volatile struct Queue* queue);


#endif /* QUEUE_H_ */
