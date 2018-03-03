// StaticSDDS.h - Header file for an example of a static Self-Describing-Data-Stream
// MIT License - 2018 - Charles Machalow
#pragma once

#include <stdint.h>

/// <summary>
/// Make a general purpose buffer
/// </summary>
uint8_t ___gpBuffer[8192] = { 0 };
bool ___gpBufferInUse = false;

// XML Pieces
#define START_XML "<cFList>"
#define XML_FIELD "<field type=\"%s\" token=\"%s\">%s</field>"
#define END_XML "</cFList>"

// Field Types
#define INTEGER_S "Integer"
#define BOOL_S "Boolean"
#define STRING_S "String"

/// <summary>
/// Private method to get the gPBuffer
/// </summary>
/// <returns></returns>
static inline uint8_t* __getGpBuffer()
{
	assert(!___gpBufferInUse);
	___gpBufferInUse = true;
	return ___gpBuffer;
}/// <summary>

/// Private method to put the gPBuffer
/// </summary>
static inline void __putGpBuffer()
{
	assert(___gpBufferInUse);
	___gpBufferInUse = false;
}

/// <summary>
/// Grab the gpbuf size
/// </summary>
/// <returns>size of the gpbuf</returns>
static inline size_t getGpBufferSize()
{
	return sizeof(___gpBuffer);
}

// General Purpose Buffer Macros
#define GET_GP_BUF() __getGpBuffer(); {
#define PUT_GP_BUF() __putGpBuffer(); }

/// <summary>
/// convert a given string into an xml-safe string and place it in gpBuffer
/// </summary>
static void stringToXmlSafeInGpBuffer(char* data);

/// <summary>
/// Convert a given xml safe string to a normal string and place it in gpBuffer
/// </summary>
static void xmlSafeToStringInGpBuffer(char* data, size_t len);

/// <summary>
/// Add a string field to an xml buffer
/// </summary>
static void addStringFieldToBuffer(uint8_t* buf, size_t bufSize, char* data, char* tokenId, size_t* offset);

/// <summary>
/// Add an unsigned numeric field to an xml buffer
/// </summary>
static void addUnsignedFieldToBuffer(uint8_t* buf, size_t bufSize, uint64_t data, char* tokenId, size_t* offset);

/// <summary>
/// Add a bool field to an xml buffer
/// </summary>
static void addBoolFieldToBuffer(uint8_t* buf, size_t bufSize, bool data, char* tokenId, size_t* offset);

/// <summary>
/// Adds a string to a given buffer via memcpy
/// </summary>
static void addStringToBuffer(uint8_t* buf, size_t bufSize, size_t* offset, char* str, size_t len);

// Macros for creating a CFList
#define START_CFLIST(buf, bufSize) { uint8_t* __buf = buf; size_t __bufSize = bufSize; size_t __offset = 0; addStringToBuffer(buf, bufSize, &__offset, START_XML, strlen(START_XML));
#define END_CFLIST() addStringToBuffer(__buf, __bufSize, &__offset, END_XML, strlen(END_XML)); }

// Macros for adding fields to a CFList
#define ADD_CFLIST_UNSIGNED_FIELD(tokenId, data) addUnsignedFieldToBuffer(__buf, __bufSize, data, tokenId, &__offset)
#define ADD_CFLIST_SIGNED_FIELD(tokenId, data) addSignedFieldToBuffer(__buf, __bufSize, data, tokenId, &__offset)
#define ADD_CFLIST_BOOL_FIELD(tokenId, data) addBoolFieldToBuffer(__buf, __bufSize, data, tokenId, &__offset)
#define ADD_CFLIST_STRING_FIELD(tokenId, data) addStringFieldToBuffer(__buf, __bufSize, data, tokenId, &__offset)

// Current Tokens
#define TOKEN_SIZE				  "A" // Size
#define TOKEN_SERIAL			  "B" // Serial
#define TOKEN_SUPPORTS_POWER	  "C" // Supports Power
