// cSDDS - Self Describing Data Steam - A way to store raw byte data in a way that is self-describing with names
// (C) - Charles Machalow via the MIT License 

#pragma warning(disable:4996) // Disable unsecure function warnings like strcpy and keep this compatible with Linux

// If on a debug build, check for memory leaks
#if _DEBUG && _WIN32
#include <vld.h>
#endif // _DEBUG && _WIN32

#include <inttypes.h>

// Local includes
#include "Memory.h"

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
bool addField(SDDS *sdds, char* fieldName, uint32_t fieldSize, BYTE* rawField, BYTE fieldStrModifier)
{
	initialize(sdds);

	// Copy name over
	char *copiedFieldName = NULL;
	if (!newStrCopy(&copiedFieldName, fieldName))
	{
		return false;
	}

	// Copy raw data
	BYTE *copiedRawField = NULL;
	if (!newRawCopy(&copiedRawField, rawField, fieldSize))
	{
		// free already allocated
		free(copiedFieldName);
		return false;
	}

	// Add size to list
	if (!addTo32BitArray(&sdds->FieldSizes, sdds->FieldCount + 1, fieldSize))
	{
		// free already allocated
		free(copiedFieldName);
		free(copiedRawField);
		return false;
	}

	// Add field str modifier to the list
	if (!addTo8BitArray(&sdds->FieldStrModifiers, sdds->FieldCount + 1, fieldStrModifier))
	{
		// free already allocated
		free(copiedFieldName);
		free(copiedRawField);
		return false;
	}

	// Only set and increment the FieldCount if everything went well.
	sdds->FieldNames[sdds->FieldCount] = copiedFieldName;
	sdds->Fields[sdds->FieldCount] = copiedRawField;

	sdds->FieldCount++;
	return true; 
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
	stringAppend(&retStr, "<Fields>\n");
	for (uint32_t i = 0; i < sdds->FieldCount; i++)
	{
		stringAppend(&retStr, "<Field FieldName=\"");
		stringAppend(&retStr, sdds->FieldNames[i]);
		stringAppend(&retStr, "\" ");
		stringAppend(&retStr, "FieldSize=");
		char buf[BUFSIZ] = "\0";
		sprintf(buf, "%u", sdds->FieldSizes[i]);
		stringAppend(&retStr, buf);
		stringAppend(&retStr, " ");
		stringAppend(&retStr, "FieldModifier=");
		memset(&buf, 0, sizeof(buf));
		sprintf(buf, "%d", sdds->FieldStrModifiers[i]);
		stringAppend(&retStr, buf);
		stringAppend(&retStr, " >");

		// Add raw buffer data
		for (uint32_t j = 0; j < roundToByte(sdds->FieldSizes[i]); j++)
		{
			memset(&buf, 0, sizeof(buf));
			sprintf(buf, "%02X", sdds->Fields[i][j]);
			stringAppend(&retStr, buf);
		}
		stringAppend(&retStr, "</Field>\n");
	}
	stringAppend(&retStr, "</Fields>\n");

	return retStr;
}

char* toString(SDDS *sdds)         // Method to parse the SDDS
{
	char* retStr = NULL;
	for (uint32_t i = 0; i < sdds->FieldCount; i++)
	{
		stringAppend(&retStr, sdds->FieldNames[i]);
		stringAppend(&retStr, "\n");
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
	addField(&s, "C", cStrLen(c) * 8, (BYTE*)c, 0);

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

// Compile / Run / Delete on Linux:
// gcc Source.c -std=c99 -lm && ./a.out && rm a.out


// Overall Todos:
/*
- Dyanically allocate memory for Fields/FieldNames
- Add getters and setters to fields by name
  - Gets the raw memory...
  - Add method to get field size, modifier by name
- Implement usage of FieldStrModifiers, and make toString() use it.
- Add checks for not duplicating field names
- Add way to go 'toBytes' and get a native byte-buffer representation of just the data (without names, etc)
- Add way to create from xml
- Add support for nesting

--> Then we have -> Decent parity with the struct functionality and serialization!
*/