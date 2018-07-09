/*
 * array.c
 *
 *  Created on: Jun 14, 2018
 *      Author: braddock.bresnahan
 */

#include "array.h"
#include <stdlib.h>
#include <string.h>

/// Definitions

ARY_ErrorState ARY_CheckError(struct Array* ary){
	if (ary == NULL) return ARRAY_ERROR_NULL_ARRAY;
	if (ary->error != ARRAY_ERROR_NONE) return ary->error;
	if (ary->typeSize < 0){
		return ary->error = ARRAY_ERROR_INVALID_TYPE_SIZE;
	}
	else if (ary->size < 0){
		return ary->error = ARRAY_ERROR_INVALID_ARRAY_SIZE;
	}
	else if (ary->capacity < 0){
		return ary->error = ARRAY_ERROR_CAPACITY_OVERFLOW;
	}

	return ARRAY_ERROR_NONE;
}

ARY_ErrorState ARY_ResetError(struct Array* ary){
	if (ary == NULL) return ARRAY_ERROR_NULL_ARRAY;
	ary->error = ARRAY_ERROR_NONE;
	return ARY_CheckError(ary);
}

// Name: ARY_GrowCapacity
// Desc: Grows the capacity of the given array.
// Input: (struct Array*) ary: The array to grow the capacity of.
// Return: (OPSTATE) If the function or it's inputs had an error.
static OPSTATE ARY_GrowCapacity(struct Array* ary){
	char* temp;
	if (ARY_CheckError(ary)) return OPSTATE_FAILURE;
	if ((ary->capacity * 2) < 0){
		ary->error = ARRAY_ERROR_CAPACITY_OVERFLOW;
		return OPSTATE_FAILURE;
	}

	temp = ary->data;
	ary->data = malloc(ary->typeSize * (ary->capacity *= 2));
	memcpy(ary->data, temp, ary->typeSize * ary->size);
	free(temp);

	return OPSTATE_SUCCESS;
}

OPSTATE ARY_InitArray(struct Array* ary, int typeSize){
	if (ary == NULL) return OPSTATE_FAILURE;

	ary->error = ARRAY_ERROR_NONE;
	if (typeSize < 0){
		typeSize = ARRAY_TYPESIZE_DEFAULT;
		ary->error = ARRAY_ERROR_INVALID_TYPE_SIZE;
	}
	ary->data = malloc(typeSize * ARRAY_CAPACITY_DEFAULT);
	ary->typeSize = typeSize;
	ary->size = 0;
	ary->capacity = ARRAY_CAPACITY_DEFAULT;

	if (ary->error)
		return OPSTATE_FAILURE;
	else
		return OPSTATE_SUCCESS;
}

OPSTATE ARY_InitArrayAdv(struct Array* ary, int typeSize, const void* defValue, int size){
	int capacity = ARRAY_CAPACITY_DEFAULT;
	char* data;  // Alias for ary->data
	const char* value; // Alias for defValue
	int inc;
	if (ary == NULL) return OPSTATE_FAILURE;

	ary->error = ARRAY_ERROR_NONE;
	if (size < 0){
		size = ARRAY_SIZE_DEFAULT;
		ary->error = ARRAY_ERROR_INVALID_ARRAY_SIZE;
	}
	if (typeSize < 0){
		typeSize = ARRAY_TYPESIZE_DEFAULT;
		ary->error = ARRAY_ERROR_INVALID_TYPE_SIZE;
	}
	while (size > capacity) capacity *= 2;
	ary->data = malloc(typeSize * capacity);
	ary->typeSize = typeSize;
	ary->size = size;
	ary->capacity = capacity;
	if (defValue != NULL){
		data = ary->data;
		value = defValue;
		inc = 0;
		for (int i = 0; i < ary->size; i++){
			inc = ary->typeSize * i;
			for (int j = 0; j < ary->typeSize; i++){ // Copies byte by byte
				data[inc + j] = value[j];
			}
		}
	}

	if (ary->error)
		return OPSTATE_FAILURE;
	else
		return OPSTATE_SUCCESS;
}

OPSTATE ARY_CloseArray(struct Array* ary){
	if (ary == NULL) return OPSTATE_FAILURE;
	if (ary->data == NULL) return OPSTATE_FAILURE;
	free(ary->data);
	return OPSTATE_SUCCESS;
}

OPSTATE ARY_CloneArray(struct Array* original, struct Array* clone){
	if (clone == NULL) return OPSTATE_FAILURE;

	if (!ARY_CheckError(original)){ // If no error
		clone->data = malloc(original->typeSize * original->capacity);
		clone->typeSize = original->typeSize;
		clone->size = original->size;
		clone->capacity = original->capacity;
		clone->error = ARRAY_ERROR_NONE; // Being here implies there is no error
		memcpy(clone->data, original->data, clone->size * clone->typeSize);
	}
	else{
		ARY_InitArray(clone, ARRAY_TYPESIZE_DEFAULT);
		clone->error = ARRAY_ERROR_UNSUCCESSFUL_CLONE;
	}

	if (clone->error)
		return OPSTATE_FAILURE;
	else
		return OPSTATE_SUCCESS;
}

char ARY_IsEmpty(struct Array* ary){
	if (ARY_CheckError(ary)) return 1; // An errored array will always be treated as empty.
	return ary->size == 0;
}

OPSTATE ARY_Add(struct Array* ary, const void* value){
	if (ARY_CheckError(ary)) return OPSTATE_FAILURE; // If error, return
	if (value == NULL){
		ary->error = ARRAY_ERROR_INVALID_VALUE_POINTER;
		return OPSTATE_FAILURE;
	}

	while (ary->size >= ary->capacity){ // Will grow until ary has the right capacity.
		if (ARY_GrowCapacity(ary) == OPSTATE_FAILURE)
			return OPSTATE_FAILURE; // Possible for error to occur in GrowCapacity (capacity overflow).
	}
	memcpy(ary->data + (ary->size * ary->typeSize), value, ary->typeSize);
	ary->size++;

	return OPSTATE_SUCCESS;
}

OPSTATE ARY_Remove(struct Array* ary, void* value){
	if (ARY_CheckError(ary)) return OPSTATE_FAILURE; // If error, return
	if (ARY_IsEmpty(ary))
		return OPSTATE_FAILURE;

	// Remove
	ary->size--;
	// Check weather to fill value
	if (value == NULL) return OPSTATE_SUCCESS;
	// Fill value
	memcpy(value, ary->data + (ary->size * ary->typeSize), ary->typeSize);

	return OPSTATE_SUCCESS;
}

OPSTATE ARY_Insert(struct Array* ary, int index, const void* value){
	if (ARY_CheckError(ary)) return OPSTATE_FAILURE; // If error, return
	if (index < 0 || index > ary->size){
		ary->error = ARRAY_ERROR_INDEX_OUT_OF_BOUNDS;
		return OPSTATE_FAILURE;
	}
	if (value == NULL){
		ary->error = ARRAY_ERROR_INVALID_VALUE_POINTER;
		return OPSTATE_FAILURE;
	}

	while (ary->size >= ary->capacity){ // Will grow until ary has the right capacity.
		if (ARY_GrowCapacity(ary) == OPSTATE_FAILURE)
			return OPSTATE_FAILURE; // Possible for error to occur in GrowCapacity (capacity overflow). Sets error state internally.
	}
	for (int i = ary->size; i > index; i--){ // Move over existing values
		memcpy(ary->data + (ary->typeSize * i), ary->data + (ary->typeSize * (i - 1)), ary->typeSize);
	}
	memcpy(ary->data + (ary->typeSize * index), value, ary->typeSize); // Write value
	ary->size++;

	return OPSTATE_SUCCESS;
}

OPSTATE ARY_RemoveAt(struct Array* ary, int index, void* value){
	if (ARY_CheckError(ary)) return OPSTATE_FAILURE; // If error, return
	if (ARY_IsEmpty(ary)){
		return OPSTATE_FAILURE;
	}
	if (index < 0 || index >= ary->size){
		ary->error = ARRAY_ERROR_INDEX_OUT_OF_BOUNDS;
		return OPSTATE_FAILURE;
	}

	// Save value
	if (value != NULL)
		memcpy(value, ary->data + (ary->typeSize * index), ary->typeSize);
	// Remove
	ary->size--;
	for (int i = index; i < ary->size; i++){
		memcpy(ary->data + (ary->typeSize * i), ary->data + (ary->typeSize * (i + 1)), ary->typeSize);
	}

	return OPSTATE_SUCCESS;
}

int ARY_IndexOf(struct Array* ary, const void* value){
	if (ARY_CheckError(ary)) return ARRAY_INDEXOF_NOTFOUND;
	if (value == NULL){
		ary->error = ARRAY_ERROR_INVALID_VALUE_POINTER;
		return ARRAY_INDEXOF_NOTFOUND;
	}

	for (int i = 0; i < ary->size; i++){
		if (memcmp(ary->data + (ary->typeSize * i), value, ary->typeSize)){
			return i;
		}
	}

	return ARRAY_INDEXOF_NOTFOUND;
}

OPSTATE ARY_Clear(struct Array* ary){
	if (ARY_CheckError(ary)) return OPSTATE_FAILURE;

	free(ary->data);
	return ARY_InitArray(ary, ary->typeSize);
}
