/*
 * array.h
 *
 *  Created on: Jun 14, 2018
 *      Author: braddock.bresnahan
 */

#ifndef ARRAY_H_
#define ARRAY_H_

/// Shared Code

#ifndef OPSTATE_
#define OPSTATE_
typedef enum{
	OPSTATE_SUCCESS = 0,
	OPSTATE_FAILURE,
} OPSTATE;
#endif

/// Macros

// Hard Macros (do not change)
#define ARRAY_INDEXOF_NOTFOUND -1

// Soft Macros (feel free to change)
#define ARRAY_CAPACITY_DEFAULT 4 // In array elements. Value MUST be POSITIVE.
#define ARRAY_TYPESIZE_DEFAULT 1 // In bytes           Value MUST be ABOVE 0.
#define ARRAY_SIZE_DEFAULT     0 // In array elements. Value MUST be POSITIVE; value must be equal to or greater than ARRAY_CAPACITY_DEFAULT.

/// Enums

typedef enum{
	ARRAY_ERROR_NONE = 0, // No error state should always be 0
	ARRAY_ERROR_INVALID_TYPE_SIZE,
	ARRAY_ERROR_INVALID_ARRAY_SIZE,
	ARRAY_ERROR_CAPACITY_OVERFLOW,
	ARRAY_ERROR_UNSUCCESSFUL_CLONE,
	ARRAY_ERROR_INVALID_VALUE_POINTER,
	ARRAY_ERROR_INDEX_OUT_OF_BOUNDS,
	ARRAY_ERROR_NULL_ARRAY,
} ARY_ErrorState;

/// Declarations

struct Array{
	void* data;
	int typeSize;
	int size;
	int capacity;
	ARY_ErrorState error;
};

/* Notes */
/* You should always close an array before it falls out of scope.
 * You should never init an array twice before closing. Close the existing array before initializing a second time. */

// Name: ARY_CheckError
// Desc: Checks and updates the error state of the given array. Also checks if the given
//       array is NULL. This function does not reset the error state if there is already
//       an error.
// Input: (struct Array*) ary: The array to check and manipulate.
// Return: (ARY_ErrorState) The final error state of the array.
ARY_ErrorState ARY_CheckError(struct Array* ary);
// Name: ARY_ResetError
// Desc: Resets the error state of the given array. This function also runs a final
//       CheckError on the array before returning.
// Input: (struct Array*) ary: The array to reset the error state of.
// Return: (ARY_ErrorState) The final error state of the array.
ARY_ErrorState ARY_ResetError(struct Array* ary);

// Name: ARY_InitArray
// Desc: Initializes a new array with the given type size and stores it in the given array.
// Input: (struct Array*) ary: The container to store the newly created array.
// Input: (int)      typeSize: The type size to give the new array.
// Return: (OPSTATE) If the function or it's inputs had an error.
OPSTATE ARY_InitArray(struct Array* ary, int typeSize);
// Name: ARY_InitArrayAdv
// Desc: Initializes a new array.
// Input: (struct Array*)    ary: The container to store the array.
//        (int)         typeSize: The typeSize of the array.
//        (const void*) defValue: The default value of the array.
//        (int)             size: The number of starting elements in the array; their
//                                starting value being defValue.
// Return: (OPSTATE) If the function or it's inputs had an error.
OPSTATE ARY_InitArrayAdv(struct Array* ary, int typeSize, const void* defValue, int size);
// Name: ARY_CloseArray
// Desc: Closes the array. To use the array again, the array needs to be initialized
//       again.
// Input: (struct Array*) ary: The array to close.
// Return: (OPSTATE) If the function or it's inputs had an error.
OPSTATE ARY_CloseArray(struct Array* ary);
// Name: ARY_CloneArray
// Desc: Makes a clone of the given original array and stores it in the clone array.
// Input: (struct Array*) original: The array to clone.
//        (struct Array*)    clone: The container to store the new clone.
// Return: (OPSTATE) If the function or it's inputs had an error
OPSTATE ARY_CloneArray(struct Array* original, struct Array* clone);

// Name: ARY_IsEmpty
// Desc: Returns if the given array has no elements.
// Input: (struct Array*) ary: The array to check for emptiness.
// Return: (char) True if the array is empty, else false. Function will always return a
//                value, even if there is an error; in which case, the value will be true.
char ARY_IsEmpty(struct Array* ary);

// Name: ARY_Add
// Desc: Adds the given value to the given array.
// Input: (struct Array*) ary: The array to add to.
//        (const void*) value: the value to add.
// Return: (OPSTATE) If the function or it's inputs had an error.
OPSTATE ARY_Add(struct Array* ary, const void* value);
// Name: ARY_Remove
// Desc: Removes the last element from the given array.
// Input: (struct Array*) ary: The array to remove from.
//        (void*)       value: The container to store the removed value. If NULL, the
//                             value will not be stored.
// Return: (OPSTATE) If the function or it's inputs had an error.
OPSTATE ARY_Remove(struct Array* ary, void* value);
// Name: ARY_Insert
// Desc: Inserts the given value into the given array at the given index.
// Input: (struct Array*) ary: The array to insert into.
//        (int)         index: The index to insert the value.
//        (const void*) value: The value to insert.
// Return: (OPSTATE) If the function or it's inputs had an error.
OPSTATE ARY_Insert(struct Array* ary, int index, const void* value);
// Name: ARY_RemoveAt
// Desc: Removes the element in the given array at the given index.
// Input: (struct Array*) ary: The array to remove from.
//        (int)         index: The index of the element to remove.
//        (void*)       value: The container to store the removed value. If NULL, the
//                             value will not be stored.
// Return: (OPSTATE) If the function or it's inputs had an error.
OPSTATE ARY_RemoveAt(struct Array* ary, int index, void* value);

// Name: ARY_IndexOf
// Desc: Returns the index of the matching value of the given value in the given array.
// Input: (struct Array*) ary: The array to comb.
//        (const void*) value: the value to search for.
// Return: (int) The index of the found value in the array. Returns -1 if not found.
int ARY_IndexOf(struct Array* ary, const void* value);

// Name: ARY_Clear
// Desc: Clears the given array of all elements.
// Input: (struct Array*) ary: The array to clear.
// Return: (OPSTATE) If the function or it's inputs had an error.
OPSTATE ARY_Clear(struct Array* ary);


#endif /* ARRAY_H_ */
