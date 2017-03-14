// Implementation file for Memory operations for cSSDS
// (C) - Charles Machalow via the MIT License 

#include "Memory.h"

/*
*
* Functions relating to C Strings
*
*/

size_t cStrLen(char *s)
{
	if (s)
	{
		return strlen(s);
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

	size_t origLen = cStrLen(origStr);
	size_t newLen = cStrLen(newStr);
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
