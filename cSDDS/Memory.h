// Header file for Memory operations for cSSDS
// (C) - Charles Machalow via the MIT License 

#pragma once

#pragma warning(disable:4996) // Disable unsecure function warnings like strcpy and keep this compatible with Linux

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef char BYTE;

/*
*
* Functions relating to C Strings
*
*/

/// <summary>
/// A smart strlen function that will return 0 for a null po9inter
/// </summary>
size_t cStrLen(char *s);

/// <summary>
/// Appends newStr to origStr. Returns true on success.
/// </summary>
bool stringAppend(char **pOrigStr, char *newStr);

/// <summary>
/// Copies old into new via allocation. Returns true on success.
/// </summary>
bool newStrCopy(char **pNewName, char *oldName);

/*
*
* Functions relating to raw memory
*
*/

/// <summary>
/// Round up to nearest byte for the given number of bits
/// </summary>
uint32_t roundToByte(uint32_t bits);

/// <summary>
/// Copy over from old into new via allocation. Returns true on success.
/// </summary>
bool newRawCopy(BYTE **pNewName, BYTE *oldName, uint32_t fieldSize);