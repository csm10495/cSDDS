// Header file for Memory operations for cSSDS
// (C) - Charles Machalow via the MIT License 

#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

