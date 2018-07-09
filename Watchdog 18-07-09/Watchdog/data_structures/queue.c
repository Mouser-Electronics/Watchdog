/*
 * queue.c
 *
 *  Created on: Jul 9, 2018
 *      Author: braddock.bresnahan
 */

#include "queue.h"
#include "stdlib.h"
#include "string.h"

/// Functions Definitions

static void QUE_GrowCapacity(volatile struct Queue* queue){
	void* temp;
	uint start, end, byteCap;

	start = queue->front;
	end = queue->back;
	byteCap = queue->byteCapacity;
	temp = queue->data;

	// Sets queue up to allow external modification immediately (The queue can be modified WHILE growing in capacity)
	queue->front = -queue->typeSize;
	queue->back = (queue->size - 1) * queue->typeSize;
	queue->capacity *= 2;
	queue->byteCapacity *= 2;
	queue->data = malloc(queue->byteCapacity);

	uint i = 0;
	while (start != end){
		start = (start + queue->typeSize) % byteCap;
		memcpy((queue->data + i), (temp + start), queue->typeSize);
		i += queue->typeSize;
	}
	free(temp);
}

void QUE_InitQueue(volatile struct Queue* const queue, uint typeSize){
	queue->data = malloc(typeSize * QUEUE_CAPACITY_DEFAULT);
	queue->typeSize = typeSize;
	queue->size = 0;
	queue->capacity = QUEUE_CAPACITY_DEFAULT;
	queue->byteCapacity = queue->typeSize * queue->capacity;
	queue->front = queue->back = -queue->typeSize;
}

void QUE_CloseQueue(volatile struct Queue* queue){
	if (queue->data != NULL)
		free(queue->data);
	queue->data = NULL;
}

void QUE_CloneQueue(volatile struct Queue* original, volatile struct Queue* const clone){
	clone->data = malloc(original->byteCapacity);
	clone->typeSize = original->typeSize;
	clone->size = original->size;
	clone->capacity = original->capacity;
	clone->byteCapacity = original->byteCapacity;
	clone->front = original->front;
	clone->back = original->back;
	memcpy(clone->data, original->data, clone->byteCapacity);
}

void QUE_CopyQueue(volatile struct Queue* original, volatile struct Queue* const copy){
	uint cursor;

	cursor = original->front;
	while (cursor != original->back){
		cursor = (cursor + original->typeSize) % original->byteCapacity;
		while (copy->size >= copy->capacity){
			QUE_GrowCapacity(copy);
		}
		copy->size++;
		copy->back = (copy->back + copy->typeSize) % copy->byteCapacity;
		memcpy((copy->data + copy->back), (original->data + cursor), copy->typeSize);
	}
}

void QUE_Enqueue(volatile struct Queue* queue, const void* value){
	while (queue->size >= queue->capacity){
		QUE_GrowCapacity(queue);
	}
	queue->size++;
	queue->back = (queue->back + queue->typeSize) % queue->byteCapacity;
	memcpy((queue->data + queue->back), value, queue->typeSize);
}

bool QUE_Dequeue(volatile struct Queue* queue, void* value){
	if (queue->size == 0) return queue->isBusy = false;
	queue->size--;
	queue->front = (queue->front + queue->typeSize) % queue->byteCapacity;
	memcpy(value, (queue->data + queue->front), queue->typeSize);

	return true;
}

void QUE_Clear(volatile struct Queue* queue){
	queue->size = 0;
	queue->front = queue->back = -queue->typeSize;
}
