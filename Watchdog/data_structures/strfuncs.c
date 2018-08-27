
#include "strfuncs.h"

/// Macros

#ifndef NULL
#define NULL 0
#endif

/// Functions

uint32_t STR_GetSize(const string cstr){
	uint32_t count;
	if (cstr == NULL) return 0;

	count = -1; // 0xffffffff
	while (cstr[++count] != '\0'); // First count value is 0

	return count;
}

int STR_Compare(const string cstr1, const string cstr2){
	uint32_t count;
	if (cstr1 == NULL || cstr2 == NULL) return 0;

	count = 0;
	while (cstr1[count] != '\0' && cstr2[count] != '\0'){
		if (cstr1[count] != cstr2[count]) break;
		count++;
	}
	return cstr1[count] - cstr2[count];
}

bool STR_Contains(const string cstr, const string substr){
	uint32_t index;
	uint32_t count;
	if (cstr == NULL || substr == NULL) return false;
	
	index = 0;
	count = 0;
	while (cstr[index] != '\0' && substr[count] != '\0'){
		if (cstr[index++] != substr[count++])
			count = 0;
	}
	if (substr[count] == '\0') return true;
	
	return false;
}

bool STR_Copy(string dst, const string src){
	uint32_t count;
	if (dst == NULL || src == NULL) return false;
	if (dst == src) return true; // Yah, sure; I TOTALLY copied the string into itself. Yep, uh-huh, yah.
	
	count = -1; // 0xffffffff
	while (src[++count] != '\0'){
		dst[count] = src[count];
	}
	dst[count] = '\0';
	
	return true;
}

bool STR_CharCat(string cstr, char value){
	uint32_t size;
	if (cstr == NULL) return false;

	size = STR_GetSize(cstr);
	cstr[size++] = value;
	cstr[size] = '\0';

	return true;
}

bool STR_Cat(string cstr, const string value){
	uint32_t size;
	uint32_t index;
	if (cstr == NULL || value == NULL) return false;

	size = STR_GetSize(cstr);
	index = -1; // 0xffffffff
	while (value[++index] != '\0'){
		cstr[size++] = value[index];
	}
	cstr[size] = '\0';

	return true;
}

bool STR_Stack(string cstr, const string val1, const string val2, const string val3, const string val4){
	const string subject = NULL;
	uint32_t state = 0;
	uint32_t size;
	uint32_t index;
	if (cstr == NULL) return false;

	size = STR_GetSize(cstr);
	/****/START:/****/
	switch (state){
		case 0: subject = val1; state++; break;
		case 1: subject = val2; state++; break;
		case 2: subject = val3; state++; break;
		case 3: subject = val4; state++; break;
		case 4: return true;
		default: return false;
	}
	if (subject == NULL) /* ^^ */goto START;/* ^^ */
	index = -1;
	while (subject[++index] != '\0'){
		cstr[size++] = subject[index];
	}
	cstr[size] = '\0';
	/* ^^ */goto START;/* ^^ */
}

bool STR_CharInsert(string cstr, uint32_t index, char value){
	uint32_t size = STR_GetSize(cstr);
	if (cstr == NULL || index > size) return false;


	for (int i = size++; i > index; i--){
		cstr[i] = cstr[i - 1];
	}
	cstr[index] = value;
	cstr[size] = '\0';

	return true;
}

bool STR_Insert(string cstr, uint32_t index, const string value){
	uint32_t size = STR_GetSize(cstr);
	uint32_t valSize;
	if (cstr == NULL || value == NULL || index > size) return false;

	valSize = STR_GetSize(value);
	for (int i = size + valSize - 1; i > index; i--){
		cstr[i] = cstr[i - valSize];
	}
	for (int i = 0; i < valSize; i++){
		cstr[index + i] = value[i];
	}
	cstr[size + valSize] = '\0';

	return true;
}

bool STR_Split(string dst, string src, char del){
	uint32_t index;
	if (dst == NULL || src == NULL || del == '\0') return false;

	index = STR_IndexOf(src, del);
	if (index == STRING_INDEX_NOT_FOUND_VALUE) return false;
	if (!STR_SubStr(dst, src, index + 1, (STR_GetSize(src) - index)))	return false;
	src[index + 1] = '\0';

	return true;
}

bool STR_SplitAt(string dst, string src, uint32_t index){
	uint32_t size = STR_GetSize(src);
	if (dst == NULL || src == NULL || index > size) return false;

	if (!STR_SubStr(dst, src, index, size - index)) return false;
	src[index] = '\0';

	return true;
}

bool STR_SubStr(string dst, const string src, uint32_t start, uint32_t count){
	if (dst == NULL || src == NULL || (start + count) > STR_GetSize(src)) return false;

	for (int i = 0; i < count; i++){
		dst[i] = src[start + i];
	}
	dst[count] = '\0';

	return true;
}

uint32_t STR_IndexOf(const string cstr, char del){
	uint32_t index;
	if (cstr == NULL) return STRING_INDEX_NOT_FOUND_VALUE;

	index = -1; // 0xffffffff
	while (cstr[++index] != '\0'){
		if (cstr[index] == del) return index;
	}

	return STRING_INDEX_NOT_FOUND_VALUE;
}

bool STR_ClearString(string cstr){
	if (cstr == NULL) return false;
	
	cstr[0] = '\0';
	
	return true;
}

bool STR_Stringify(char* ary, uint32_t length){
	if (ary == NULL) return false;
	
	ary[length] = '\0';
	
	return true;
}

void STR_NumToHex8(string dst, char num){
	if (dst == NULL) return;
	
	for (int i = SIZEOF_CHAR_IN_HEX - 1; i >= 0; i--){
		switch (num & 0xf){
			case 0x0: dst[i] = '0'; break;
			case 0x1: dst[i] = '1'; break;
			case 0x2: dst[i] = '2'; break;
			case 0x3: dst[i] = '3'; break;
			case 0x4: dst[i] = '4'; break;
			case 0x5: dst[i] = '5'; break;
			case 0x6: dst[i] = '6'; break;
			case 0x7: dst[i] = '7'; break;
			case 0x8: dst[i] = '8'; break;
			case 0x9: dst[i] = '9'; break;
			case 0xa: dst[i] = 'A'; break;
			case 0xb: dst[i] = 'B'; break;
			case 0xc: dst[i] = 'C'; break;
			case 0xd: dst[i] = 'D'; break;
			case 0xe: dst[i] = 'E'; break;
			case 0xf: dst[i] = 'F'; break;
		}
		num = num >> SIZEOF_HEX_IN_BITS;
	}
	dst[SIZEOF_CHAR_IN_HEX] = '\0';
}

void STR_NumToHex16(string dst, unsigned short num){
	if (dst == NULL) return;
	
	for (int i = SIZEOF_SHORT_IN_HEX - 1; i >= 0; i--){
		switch (num & 0xf){
			case 0x0: dst[i] = '0'; break;
			case 0x1: dst[i] = '1'; break;
			case 0x2: dst[i] = '2'; break;
			case 0x3: dst[i] = '3'; break;
			case 0x4: dst[i] = '4'; break;
			case 0x5: dst[i] = '5'; break;
			case 0x6: dst[i] = '6'; break;
			case 0x7: dst[i] = '7'; break;
			case 0x8: dst[i] = '8'; break;
			case 0x9: dst[i] = '9'; break;
			case 0xa: dst[i] = 'A'; break;
			case 0xb: dst[i] = 'B'; break;
			case 0xc: dst[i] = 'C'; break;
			case 0xd: dst[i] = 'D'; break;
			case 0xe: dst[i] = 'E'; break;
			case 0xf: dst[i] = 'F'; break;
		}
		num = num >> SIZEOF_HEX_IN_BITS;
	}
	dst[SIZEOF_SHORT_IN_HEX] = '\0';
}

uint8_t STR_HexToInt8(const string hex){
	char acc;
	uint32_t size;
	if (hex == NULL) return 0;
	
	size = STR_GetSize(hex);
	if (size >= 2 && (hex[1] == 'x' || hex[1] == 'X')){ // Formatted hex
		hex = hex + 2; // Get rid of 0x at front
		size = size - 2; // Adjust size
	}
	if (size > 2) // The size may have been adjusted
		size = SIZEOF_CHAR_IN_HEX; // size capped at 2

	acc = 0;
	for (int i = 0; i < size; i++){
		acc = acc << SIZEOF_HEX_IN_BITS;
		switch (hex[i]){
			case '0': break;
			case '1': acc |= 0x1; break;
			case '2': acc |= 0x2; break;
			case '3': acc |= 0x3; break;
			case '4': acc |= 0x4; break;
			case '5': acc |= 0x5; break;
			case '6': acc |= 0x6; break;
			case '7': acc |= 0x7; break;
			case '8': acc |= 0x8; break;
			case '9': acc |= 0x9; break;
			case 'a': case 'A': acc |= 0xa; break;
			case 'b': case 'B': acc |= 0xb; break;
			case 'c': case 'C': acc |= 0xc; break;
			case 'd': case 'D': acc |= 0xd; break;
			case 'e': case 'E': acc |= 0xe; break;
			case 'f': case 'F': acc |= 0xf; break;
			default: return acc; break;
		}
	}
	
	return acc;
}

uint16_t STR_HexToInt16(const string hex){
	uint16_t acc;
	if (hex == NULL) return 0;
	if (STR_GetSize(hex) < SIZEOF_SHORT_IN_HEX) return 0;

	acc = STR_HexToInt8(hex);		// 0x..AA, hex: 0xAA..
	acc = acc << 8;					// 0xAA00
	acc |= STR_HexToInt8(hex + 2);	// 0xAABB, hex: 0x..BB
	
	// acc: 0xAABB, hex: 0xAABB
	
	return acc;
}

uint32_t STR_HexToInt32(const string hex){
	uint32_t acc;
	if (STR_GetSize(hex) < SIZEOF_INT_IN_HEX) return 0;
	
	acc = STR_HexToInt8(hex);		// 0x......AA, hex: 0xAA......
	acc = acc << 8;					// 0x....AA..
	acc |= STR_HexToInt8(hex + 2);	// 0x....AABB, hex: 0x..BB....
	acc = acc << 8;					// 0x..AABB..
	acc |= STR_HexToInt8(hex + 4);	// 0x..AABBCC, hex: 0x....CC..
	acc = acc << 8;					// 0xAABBCC..
	acc |= STR_HexToInt8(hex + 6);	// 0xAABBCCDD, hex: 0x......DD
	
	// acc: 0xAABBCCDD, hex: 0xAABBCCDD
	
	return acc;
}

void STR_HexToNum(void* dst, const string hex){
	uint32_t size;
	if (dst == NULL || hex == NULL) return;
	
	size = 0;
	// Truely a while(true) loop in disguise
	/****/BEGIN:/****/
	if (hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) // Ignores any "0x"s
		/* vv */goto CONTINUE;/* vv */
	
	for (int i = 0; i < SIZEOF_CHAR_IN_HEX; i++){
		switch (hex[i]){ // Checks for valid characters
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			case 'a': case 'A': case 'b': case 'B':
			case 'c': case 'C': case 'd': case 'D': // Allows valid values to pass through
			case 'e': case 'E': case 'f': case 'F': continue;
			default: // Any other character will stop the process
				if (i == 0) /* vv */goto EXIT;/* vv */   // The only exit of the loop
				else continue;
		}
	}
	
	*(char*)dst = STR_HexToInt8(hex);
	dst++;
	size++;
	
	/****/CONTINUE:/****/
	hex = hex + 2;
	/* ^^ */goto BEGIN;/* ^^ */
	/****/EXIT:/****/
	if (STRING_PACK_NUMBER_AS_LITTLE_ENDIAN)
		STR_FlipEndian(dst - size, size);
	
	return;
}

void STR_FlipEndian(void* data, uint32_t length){
	char temp;
	uint32_t i = 0;
	uint32_t j = length - 1;
	for (; i < (length >> 1); i++, j--){
		              temp = *(char*)(data + i);	// temp = data[i]
		*(char*)(data + i) = *(char*)(data + j);	// data[i] = data[j]
		*(char*)(data + j) = temp;					// data[j] = temp
	}
}





