/*
 * queue.c
 *
 *  Created on: Jul 9, 2018
 *      Author: braddock.bresnahan
 */

#include "queue.h"
#include "stdlib.h"
#include "string.h"

/// "Private" Functions

static inline uint QUE_IncrimentF(volatile const struct Queue* queue){
	return (queue->front + queue->typeSize) % queue->byteCapacity;
}

static inline uint QUE_IncrimentB(volatile const struct Queue* queue){
	return (queue->back + queue->typeSize) % queue->byteCapacity;
}

static void QUE_GrowCapacity(volatile struct Queue* queue){
	struct Queue old;
	uint size;
	void* pcData;
	uint pcFront;
	uint pcBack;
	
	memcpy((void*)&old, (void*)queue, sizeof(struct Queue));
	// Pit Crew
	pcData  = malloc(old.byteCapacity * 2);
	pcFront = -old.typeSize;
	pcBack  = (old.size - 1) * old.typeSize;

	// Setup queue to allow external modification immediately (The queue can be modified WHILE growing in capacity) [You should't do that though]
	// The next three lines, once started, need to be done quickly. (hence the pit crew)
	queue->front = pcFront;
	queue->data  = pcData;
	queue->back  = pcBack;
	// "Wow! That change was so fast, it put all of F1 to shame!" - Not Me (it was me)
	
	queue->capacity *= 2;
	queue->byteCapacity *= 2;
	size = old.size * old.typeSize;
	for (int i = 0; i < size; i += old.typeSize){
		old.front = QUE_IncrimentF(&old);
		memcpy((queue->data + i), (old.data + old.front), old.typeSize);
	}
	free(old.data);
}

/// Function Definitions

bool QUE_InitQueue(volatile struct Queue* const queue, uint typeSize){
	if (queue == NULL) return QUEUE_FAIL;
	
	queue->data = malloc(typeSize * QUEUE_CAPACITY_DEFAULT);
	queue->typeSize = typeSize;
	queue->size = 0;
	queue->capacity = QUEUE_CAPACITY_DEFAULT;
	queue->byteCapacity = queue->typeSize * queue->capacity;
	queue->front = queue->back = -queue->typeSize;
	
	return QUEUE_SUCCESS;
}

bool QUE_CloseQueue(volatile struct Queue* queue){
	
	if (QUE_IsValid(queue)){
		free(queue->data);
		queue->data = NULL;
	}
	
	return QUEUE_SUCCESS;
}

bool QUE_CloneQueue(volatile struct Queue* const dst, volatile const struct Queue* const src){
	if (dst == NULL || QUE_IsNotValid(src)) return QUEUE_FAIL;
	
	memcpy((void*)dst, (void*)src, sizeof(struct Queue));
	dst->data = malloc(dst->byteCapacity);
	memcpy(dst->data, src->data, dst->byteCapacity);
	
	return QUEUE_SUCCESS;
}

bool QUE_CopyQueue(volatile struct Queue* const dst, volatile const struct Queue* const src){
	void* holder;
	if (QUE_IsNotValid(dst) || QUE_IsNotValid(src)) return QUEUE_FAIL;
	
	holder = dst->data;
	memcpy((void*)dst, (void*)src, sizeof(struct Queue));
	dst->data = holder;
	memcpy(dst->data, src->data, dst->byteCapacity);
	
	return QUEUE_SUCCESS;
}

bool QUE_Enqueue(volatile struct Queue* queue, const void* value){
	if (QUE_IsNotValid(queue) || value == NULL) return QUEUE_FAIL;
	
	while (queue->size >= queue->capacity){
		QUE_GrowCapacity(queue);
	}
	queue->size++;
	queue->back = QUE_IncrimentB(queue);
	memcpy((queue->data + queue->back), value, queue->typeSize);
	
	return QUEUE_SUCCESS;
}

bool QUE_Dequeue(volatile struct Queue* queue, void* value){
	if (QUE_IsEmpty(queue)) return QUEUE_FAIL; // Also checks validity
	
	queue->size--;
	queue->front = QUE_IncrimentF(queue);
	if (value != NULL)
		memcpy(value, (queue->data + queue->front), queue->typeSize);

	return QUEUE_SUCCESS;
}

bool QUE_Peek(volatile const struct Queue* queue, void* value){
	if (QUE_IsEmpty(queue) || value == NULL) return QUEUE_FAIL; // Also checks validity
	
	memcpy(value, (queue->data + QUE_IncrimentF(queue)), queue->typeSize);
	
	return QUEUE_SUCCESS;
}

bool QUE_RememberEnqueue(volatile const struct Queue* queue, void* value){
	if (QUE_IsNotValid(queue) || value == NULL) return QUEUE_FAIL;
	
	memcpy(value, (queue->data + queue->back), queue->typeSize);
	
	return QUEUE_SUCCESS;
}

bool QUE_RememberDequeue(volatile const struct Queue* queue, void* value){
	if (QUE_IsNotValid(queue) || value == NULL) return QUEUE_FAIL;
	
	memcpy(value, (queue->data + queue->front), queue->typeSize);
	
	return QUEUE_SUCCESS;
}

bool QUE_Clear(volatile struct Queue* queue){
	if (QUE_IsNotValid(queue)) return QUEUE_FAIL;
	
	queue->size = 0;
	queue->front = queue->back;
	
	return QUEUE_SUCCESS;
}
