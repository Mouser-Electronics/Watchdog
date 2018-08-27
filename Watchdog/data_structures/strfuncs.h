
#ifndef STRFUNCS_H_
#define STRFUNCS_H_

#include <stdint.h>
#include <stdbool.h>

/// Macros

#ifndef string
#define string	char*
#endif

#define STRING_INDEX_NOT_FOUND_VALUE	0xffffffff // -1

#define SIZEOF_HEX_IN_BITS	4
#define SIZEOF_CHAR_IN_HEX	2
#define SIZEOF_SHORT_IN_HEX	4
#define SIZEOF_INT_IN_HEX	8

#define STRING_PACK_NUMBER_AS_LITTLE_ENDIAN	true

/// Functions

uint32_t STR_GetSize(const string cstr);

int STR_Compare(const string cstr1, const string cstr2);
bool STR_Contains(const string cstr, const string substr);

bool STR_Copy(string dst, const string src);

bool STR_CharCat(string cstr, char value);
bool STR_Cat(string cstr, const string value);
bool STR_Stack(string cstr, const string val1, const string val2, const string val3, const string val4);

bool STR_CharInsert(string cstr, uint32_t index, char value);
bool STR_Insert(string cstr, uint32_t index, const string value);

bool STR_Split(string dst, string src, char del);
bool STR_SplitAt(string dst, string src, uint32_t index);

bool STR_SubStr(string dst, const string src, uint32_t start, uint32_t count);

uint32_t STR_IndexOf(const string cstr, char del);

bool STR_ClearString(string cstr);

bool STR_Stringify(char* ary, uint32_t length);

void STR_NumToHex8(string dst, char num);
void STR_NumToHex16(string dst, unsigned short num);

uint8_t STR_HexToInt8(const string hex);
uint16_t STR_HexToInt16(const string hex);
uint32_t STR_HexToInt32(const string hex);
inline int STR_HexToInt(const string hex){
	return (int)STR_HexToInt32(hex);
}
void STR_HexToNum(void* dst, const string hex);

void STR_FlipEndian(void* data, uint32_t length);

#endif /* STRFUNCS_H_ */
