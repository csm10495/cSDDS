// Header file for Memory operations for cSSDS (C Self Describing Data Steam)
// (C) - Charles Machalow via the MIT License 

#pragma once

#ifdef _WIN32
#pragma warning(disable:4996) // Disable unsecure function warnings like strcpy and keep this compatible with Linux
#endif // _WIN32

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t BYTE;

/*
*
* Functions relating to C Strings
*
*/

/// <summary>
/// A smart strlen function that will return 0 for a null pointer
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

/// <summary>
/// Adds the new 32bit value to the array. Returns true on success.
/// </summary>
bool addTo32BitArray(uint32_t **array32, uint32_t newSize, uint32_t newValue);

/// <summary>
/// Adds the new 8bit value to the array. Returns true on success.
/// </summary>
bool addTo8BitArray(uint8_t **array8, uint32_t newSize, uint8_t newValue);

/// <summary>
/// Reallocs pppByte to the new size and passes the pointer to the last index
/// </summary>
bool reallocPPPByte(BYTE ***pppByte, uint32_t newSize, uint8_t *newValue);