// Implementation file for Memory operations for cSSDS
// (C) - Charles Machalow via the MIT License 

#include "Memory.h"

/*
*
* Functions relating to C Strings
*
*/

uint32_t cStrLen(char *s)
{
	if (s)
	{
		return (uint32_t)strlen(s);
	}
	return 0;
}

bool stringAppend(char **pOrigStr, char *newStr)
{
	char *origStr;
	if (pOrigStr)
	{
		origStr = *pOrigStr;
	}
	else
	{
		origStr = NULL;
	}

	uint32_t origLen = cStrLen(origStr);
	uint32_t newLen = cStrLen(newStr);
	char* appendedStr = (char*)realloc(origStr, origLen + newLen + 1); // Add 1 for null char
	if (appendedStr)
	{
		memcpy(appendedStr + origLen, newStr, newLen);
		appendedStr[origLen + newLen] = '\0'; // null char
		*pOrigStr = appendedStr;
		return true;
	}
	return false;
}

bool newStrCopy(char **pNewName, char *oldName)
{
	char* newName = (char*)calloc(cStrLen(oldName) + 1, sizeof(char));
	if (newName)
	{
		strcpy(newName, oldName);
		*pNewName = newName;
		return true;
	}
	return false;
}

/*
*
* Functions relating to raw memory
*
*/

uint32_t roundToByte(uint32_t bits)
{
	return (uint32_t)ceill(bits / 8.0);
}

bool newRawCopy(BYTE **pNewName, BYTE *oldName, uint32_t fieldSize)
{
	BYTE* newName = (BYTE*)calloc(fieldSize, sizeof(BYTE));
	if (newName)
	{
		memcpy(newName, oldName, roundToByte(fieldSize));
		*pNewName = newName;
		return true;
	}
	return false;
}

bool addTo32BitArray(uint32_t **array32, uint32_t newSize, uint32_t newValue)
{
	// Allocate up 1.
	uint32_t *tmp = (uint32_t*)realloc(*array32, newSize * sizeof(uint32_t));
	if (tmp)
	{
		tmp[newSize - 1] = newValue;
		*array32 = tmp;
		return true;
	}
	return false;
}

bool addTo8BitArray(uint8_t **array8, uint32_t newSize, uint8_t newValue)
{
	// Allocate up 1.
	uint8_t *tmp = (uint8_t*)realloc(*array8, newSize * sizeof(uint8_t));
	if (tmp)
	{
		tmp[newSize - 1] = newValue;
		*array8 = tmp;
		return true;
	}
	return false;
}

bool reallocPPPByte(BYTE ***pppByte, uint32_t newSize, uint8_t* newValue)
{
	// Allocate up 1.
	uint8_t **tmp = (uint8_t**)realloc(*pppByte, newSize * sizeof(uint8_t**));
	if (tmp)
	{
		tmp[newSize - 1] = newValue;
		*pppByte = tmp;
		return true;
	}
	return false;
}
