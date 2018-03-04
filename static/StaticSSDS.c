// StaticSDDS.c - Implementation file for an example of a static Self-Describing-Data-Stream
// MIT License - 2018 - Charles Machalow

#define _CRT_SECURE_NO_WARNINGS 1

#include <assert.h>
#include <inttypes.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "StaticSDDS.h"

// XML Safe Conversions
#define XML_DOUBLE_QUOTE "&quot;"
#define XML_SINGLE_QUOTE "&apos;"
#define XML_LESS_THAN    "&lt;"
#define XML_GREATER_THAN "&gt;"
#define XML_AMPERSAND    "&amp;"

// Normal Things That Need XML Conversions
#define NORMAL_DOUBLE_QUOTE '"'
#define NORMAL_SINGLE_QUOTE '\''
#define NORMAL_LESS_THAN    '<'
#define NORMAL_GREATER_THAN '>'
#define NORMAL_AMPERSAND    '&'


static void stringToXmlSafeInGpBuffer(char* data)
{
	size_t i = 0;
	size_t gpBufOffset = 0;
	uint8_t* gpBuf = GET_GP_BUF();

	for (; i < strlen(data); i++)
	{
		char* sToAdd;
		size_t len = 0;
		switch (data[i])
		{
		case NORMAL_DOUBLE_QUOTE:
			sToAdd = XML_DOUBLE_QUOTE;
			break;
		case NORMAL_SINGLE_QUOTE:
			sToAdd = XML_SINGLE_QUOTE;
			break;
		case NORMAL_LESS_THAN:
			sToAdd = XML_LESS_THAN;
			break;
		case NORMAL_GREATER_THAN:
			sToAdd = XML_GREATER_THAN;
			break;
		case NORMAL_AMPERSAND:
			sToAdd = XML_AMPERSAND;
			break;
		default:
			sToAdd = &data[i];
			len = 1;
			break;
		}

		if (len == 0)
		{
			len = strlen(sToAdd);
		}

		addStringToBuffer(gpBuf, getGpBufferSize(), &gpBufOffset, sToAdd, len);
	}
	gpBuf[gpBufOffset] = 0;

	PUT_GP_BUF();
}

void xmlSafeToStringInGpBuffer(char* data, size_t len)
{
	if (len == 0)
	{
		len = strlen(data);
	}

	uint8_t* gpBuf = GET_GP_BUF();

	size_t idx = 0;
	size_t gpBufOffset = 0;
	for (; idx < len; idx++)
	{
		if (data[idx] == NORMAL_AMPERSAND)
		{
			// conversion to 'normal' needed
			size_t remainingDataLength = len - idx;
			assert(remainingDataLength >= 3);
			char normalChar = 0;

			if (remainingDataLength >= 3)
			{
				if (memcmp(&data[idx], XML_LESS_THAN, 3) == 0)
				{
					normalChar = NORMAL_LESS_THAN;
				}
				else if (memcmp(&data[idx], XML_GREATER_THAN, 3) == 0)
				{
					normalChar = NORMAL_GREATER_THAN;
				}

				if (normalChar)
				{
					idx += 3;
				}
			}
			if (!normalChar && remainingDataLength >= 4)
			{
				if (memcmp(&data[idx], XML_AMPERSAND, 4) == 0)
				{
					normalChar = NORMAL_AMPERSAND;
				}

				if (normalChar)
				{
					idx += 4;
				}
			}
			if (!normalChar && remainingDataLength >= 5)
			{
				if (memcmp(&data[idx], XML_DOUBLE_QUOTE, 5) == 0)
				{
					normalChar = NORMAL_DOUBLE_QUOTE;
				}
				else if (memcmp(&data[idx], XML_SINGLE_QUOTE, 5) == 0)
				{
					normalChar = NORMAL_SINGLE_QUOTE;
				}

				if (normalChar)
				{
					idx += 5;
				}
			}

			assert(normalChar); // unable to convert to normal text

			addStringToBuffer(gpBuf, getGpBufferSize(), &gpBufOffset, &normalChar, 1);
		}
		else
		{
			addStringToBuffer(gpBuf, getGpBufferSize(), &gpBufOffset, data + idx, 1);
		}
	}
	gpBuf[gpBufOffset] = 0;

	PUT_GP_BUF();
}

static void addStringToBuffer(uint8_t* buf, size_t bufSize, size_t* offset, char* str, size_t len)
{
	assert(bufSize > *offset + len);
	memcpy(buf + *offset, str, len);
	*offset += len;
}

bool getStringFieldByTokenAndPutInGpBuf(uint8_t *xmlBuf, size_t xmlBufSize, char * tokenId)
{
	char* lessThan = "<";
	size_t countOfLessThan = countACharInString(xmlBuf, xmlBufSize, lessThan[0]);

	char* token = strtok(xmlBuf, lessThan);
	size_t i = 0;
	for (; i < (countOfLessThan - 1); i++)
	{
		if (i % 2 != 0)
		{
			size_t tokenIdLoc = findAfterInStr(token, 0, "token=\"");
			assert(tokenIdLoc != -1);
			size_t endTokenIdQuoteLoc = findAfterInStr(token + tokenIdLoc, 0, "\"");
			assert(endTokenIdQuoteLoc != -1);
			endTokenIdQuoteLoc += tokenIdLoc - 1; // get before quote

			if (endTokenIdQuoteLoc - tokenIdLoc == strlen(tokenId))
			{
				if (memcmp(token + tokenIdLoc, tokenId, strlen(tokenId)) == 0)
				{
					uint8_t* gpBuf = GET_GP_BUF();
					size_t offset = 0;
					addStringToBuffer(gpBuf, getGpBufferSize(), &offset, token, strlen(token));
					PUT_GP_BUF();
					return true;
				}
			}
			printf("Current: %s\n", token);
		}
		token = strtok(NULL, lessThan);
	}
	return false;
}

size_t countACharInString(char * str, size_t len, char c)
{
	size_t count = 0;
	if (len == 0)
	{
		len = strlen(str);
	}

	size_t i = 0;
	for (; i < len; i++)
	{
		if (str[i] == c)
		{
			count += 1;
		}
	}

	return count;
}

size_t findAfterInStr(char * strToSearchIn, size_t strToSearchInLen, char *strToFind)
{
	size_t i = 0;
	if (strToSearchInLen == 0)
	{
		strToSearchInLen = strlen(strToSearchIn);
	}

	for (; i < strToSearchInLen; i++)
	{
		size_t remainingLen = strToSearchInLen - i;
		if (remainingLen > strlen(strToFind))
		{
			if (memcmp(strToSearchIn + i, strToFind, strlen(strToFind)) == 0)
			{
				return i + strlen(strToFind);
			}
		}
	}

	return -1;
}

static void addStringFieldToBuffer(uint8_t* buf, size_t bufSize, char* data, char* tokenId, size_t* offset)
{
	stringToXmlSafeInGpBuffer(data);
	uint8_t* gpBuf = GET_GP_BUF();
	int numChars = snprintf(buf + *offset, bufSize - *offset, XML_FIELD, STRING_S, tokenId, gpBuf);
	assert(numChars > 0);
	*offset += numChars;
	PUT_GP_BUF();
}

static void addUnsignedFieldToBuffer(uint8_t* buf, size_t bufSize, uint64_t data, char* tokenId, size_t* offset)
{
	uint8_t* gpBuf = GET_GP_BUF();

	int numChars = snprintf(gpBuf, getGpBufferSize(), "%"PRIu64, data);
	assert(numChars > 0);
	numChars = snprintf(buf + *offset, bufSize - *offset, XML_FIELD, INTEGER_S, tokenId, gpBuf);
	assert(numChars > 0);
	*offset += numChars;

	PUT_GP_BUF();
}

static void addSignedFieldToBuffer(uint8_t* buf, size_t bufSize, int64_t data, char* tokenId, size_t* offset)
{
	uint8_t* gpBuf = GET_GP_BUF();

	int numChars = snprintf(gpBuf, getGpBufferSize(), "%"PRIi64, data);
	assert(numChars > 0);

	numChars = snprintf(buf + *offset, bufSize - *offset, XML_FIELD, INTEGER_S, tokenId, gpBuf);
	assert(numChars > 0);
	*offset += numChars;

	PUT_GP_BUF();
}

static void addBoolFieldToBuffer(uint8_t* buf, size_t bufSize, bool data, char* tokenId, size_t* offset)
{
	char* s = NULL;
	if (data)
	{
		s = "True";
	}
	else
	{
		s = "False";
	}

	int numChars = snprintf(buf + *offset, bufSize - *offset, XML_FIELD, BOOL_S, tokenId, s);
	assert(numChars > 0);
	*offset += numChars;
}

int main()
{
	uint8_t tbuf[4096] = { 0 };

	char* testStr = "Test";

	START_CFLIST(tbuf, sizeof(tbuf));
	ADD_CFLIST_STRING_FIELD(TOKEN_SERIAL, testStr);
	ADD_CFLIST_SIGNED_FIELD(TOKEN_SIZE, -12345);
	ADD_CFLIST_BOOL_FIELD(TOKEN_SUPPORTS_POWER, true);
	END_CFLIST();

	printf("%s\n", (char*)tbuf);

	getStringFieldByTokenAndPutInGpBuf(tbuf, sizeof(tbuf), "B");

	return EXIT_SUCCESS;
}