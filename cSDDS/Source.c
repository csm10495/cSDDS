// cSDDS - Self Describing Data Steam - A way to store raw byte data in a way that is self-describing with names
// (C) - Charles Machalow via the MIT License 

#pragma warning(disable:4996) // Disable unsecure function warnings like strcpy and keep this compatible with Linux

// If on a debug build, check for memory leaks
#if _DEBUG && _WIN32
#include <vld.h>
#endif // _DEBUG && _WIN32

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <stdbool.h>

typedef char BYTE;

// Creates a new string which is a copy of the given one. Make it 1 byte larger to guarantee a null terminator no matter what
#define COPY_STRING_WITH_NEW_MEMORY(oldName, newName) char* newName = (char*)calloc(strlen(oldName) + 1, sizeof(char)); strcpy(newName, oldName) 

// Creates a new raw memory buffer and copies the old into the new
#define COPY_RAW_WITH_NEW_MEMORY(oldName, newName, fieldSize) BYTE* newName = (BYTE*)calloc(roundToByte(fieldSize), sizeof(BYTE)); memcpy(newName, oldName, roundToByte(fieldSize))

// Adds to the field modifiers array a single value
#define ADD_TO_FIELD_MODIFIERS(fieldStrModifiers, newSize, newValue) fieldStrModifiers = (BYTE*)realloc(fieldStrModifiers, newSize * sizeof(BYTE)); fieldStrModifiers[newSize-1] = newValue;

uint32_t roundToByte(uint32_t bits)
{
	return (uint32_t)ceill(bits / 8.0);
}

size_t SMART_STRLEN(char *s)
{
	if (s)
		return strlen(s);
	return 0;
}
// Adds to a given c string the other c string (append). Adds b to a
char* append(char * a, char * b)
{
	size_t alen = SMART_STRLEN(a);
	size_t blen = SMART_STRLEN(b);
	a = (char*)realloc(a, alen + blen + 1);
	memcpy(a + alen, b, blen);
	a[alen + blen] = '\0'; // null char
	return a;
}

// Self Describing Data Stream
typedef struct SDDS {
	BYTE** Fields;
	char** FieldNames;
	uint32_t* FieldSizes;
	BYTE* FieldStrModifiers;
	uint32_t FieldCount;
	bool Initialized;
} SDDS, *PSDDS;

void initialize(SDDS *sdds)
{
	if (!sdds->Initialized)
	{
		// Todo: Don't hardcode to 512 max.
		sdds->Fields = (BYTE**)calloc(512, sizeof(BYTE**));
		sdds->FieldNames = (BYTE**)calloc(512, sizeof(char**));
		sdds->FieldSizes = NULL;        // Used to know the size IN BITS of each field
		sdds->FieldStrModifiers = NULL; // Used to describe in string format
		sdds->FieldCount = 0;           // Number of fields
	}
	sdds->Initialized = true;
}

// Adds field to the SDDS
void addField(SDDS *sdds, char* fieldName, uint32_t fieldSize, BYTE* rawField, BYTE fieldStrModifier)
{
	initialize(sdds);

	// Copy name over
	COPY_STRING_WITH_NEW_MEMORY(fieldName, copiedFieldName);
	sdds->FieldNames[sdds->FieldCount] = copiedFieldName;

	COPY_RAW_WITH_NEW_MEMORY(rawField, copiedRawField, fieldSize);
	sdds->Fields[sdds->FieldCount] = copiedRawField;

	// Adds to the field sizes array a single value
	{
		uint32_t newFieldCount = sdds->FieldCount + 1;

		// Allocate up 1.
		sdds->FieldSizes = (uint32_t*)realloc(sdds->FieldSizes, newFieldCount * sizeof(uint32_t));
		sdds->FieldSizes[sdds->FieldCount] = fieldSize;
	}

	// Add one to the field modifiers array
	ADD_TO_FIELD_MODIFIERS(sdds->FieldStrModifiers, sdds->FieldCount + 1, fieldStrModifier);

	sdds->FieldCount++;
}

uint32_t getFieldCount(SDDS *sdds)
{
	return sdds->FieldCount;
}

// Returns the size in bits
uint64_t getTotalBitSize(SDDS *sdds)
{
	uint64_t totalSize = 0;
	for (uint64_t i = 0; i < sdds->FieldCount; i++)
	{
		totalSize += sdds->FieldSizes[i];
	}
	return totalSize;
}

uint64_t getTotalByteSize(SDDS *sdds)
{
	return (uint64_t)ceill(getTotalBitSize(sdds) / 8.0);
}

char* toXml(SDDS *sdds)          // Method to describe the SDDS
{
	char* retStr = NULL;
	retStr = append(retStr, "<Fields>\n");
	for (uint32_t i = 0; i < sdds->FieldCount; i++)
	{
		retStr = append(retStr, "<Field FieldName=\"");
		retStr = append(retStr, sdds->FieldNames[i]);
		retStr = append(retStr, "\" ");
		retStr = append(retStr, "FieldSize=");
		char buf[BUFSIZ] = "\0";
		sprintf(buf, "%u", sdds->FieldSizes[i]);
		retStr = append(retStr, buf);
		retStr = append(retStr, " ");
		retStr = append(retStr, "FieldModifier=");
		memset(&buf, 0, sizeof(buf));
		sprintf(buf, "%d", sdds->FieldStrModifiers[i]);
		retStr = append(retStr, buf);
		retStr = append(retStr, " >");

		// Add raw buffer data
		for (uint32_t j = 0; j < roundToByte(sdds->FieldSizes[i]); j++)
		{
			memset(&buf, 0, sizeof(buf));
			sprintf(buf, "%02X", sdds->Fields[i][j]);
			retStr = append(retStr, buf);
		}
		retStr = append(retStr, "</Field>\n");
	}
	retStr = append(retStr, "</Fields>\n");

	return retStr;
}

char* toString(SDDS *sdds)         // Method to parse the SDDS
{
	char* retStr = NULL;
	for (uint32_t i = 0; i < sdds->FieldCount; i++)
	{
		retStr = append(retStr, sdds->FieldNames[i]);
		retStr = append(retStr, "\n");
	}
	return retStr;
}

// Used to free all allocations.
void close(SDDS *sdds)
{
	sdds->Initialized = false;
	for (uint32_t i = 0; i < sdds->FieldCount; i++)
	{
		free(sdds->Fields[i]);
		free(sdds->FieldNames[i]);
	}
	free(sdds->FieldSizes);
	free(sdds->FieldStrModifiers);
	free(sdds->Fields);
	free(sdds->FieldNames);
	sdds->FieldCount = 0;
}

int main()
{
	SDDS s = { 0 };
	BYTE a[1] = { 1 };
	addField(&s, "A", 8, a, 0);

	BYTE b[6] = { 1, 2, 3, 4, 5 ,6 };
	addField(&s, "B", 48, b, 0);

	char* c = "Hello There!";
	addField(&s, "C", strlen(c) * 8, (BYTE*)c, 0);

	char* fields = toString(&s);
	printf("Fields: \n%s\n", fields);
	printf("Size in Bits:  %" PRIu64 "\n", getTotalBitSize(&s));
	printf("Size in Bytes: %" PRIu64 "\n", getTotalByteSize(&s));
	char * xml = toXml(&s);
	printf("xml:\n%s\n", xml);

	free(fields);
	free(xml);
	close(&s);

	return 1;
}


// Overall Todos:
/*
- Get rid of lazy macros
- Dyanically allocate memory for Fields/FieldNames
- Add getters and setters to fields by name
  - Gets the raw memory...
  - Add method to get field size, modifier by name
- Implement usage of FieldStrModifiers, and make toString() use it.
- Add checks for not duplicating field names
- Add checks for if malloc/etc fail
- Add way to go 'toBytes' and get a native byte-buffer representation of just the data (without names, etc)
- Add way to create from xml
- Add support for nesting

--> Then we have -> Decent parity with the struct functionality and serialization!
*/