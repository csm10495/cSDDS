// cSDDS - Self Describing Data Steam - A way to store raw byte data in a way that is self-describing with names
// (C) - Charles Machalow via the MIT License 

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

// Hmm may not need this method if we are forcing users to set their SDDS to all 0.
void initialize(SDDS *sdds)
{
	if (!sdds->Initialized)
	{
		sdds->Fields = NULL;            // List of raw fields
		sdds->FieldNames = NULL;        // List of field names
		sdds->FieldSizes = NULL;        // Used to know the size IN BITS of each field
		sdds->FieldStrModifiers = NULL; // Used to describe in string format
		sdds->FieldCount = 0;           // Number of fields
	}
	sdds->Initialized = true;
}


// Returns the a pointer to the raw data for a given field name. Also, optionally can give back the field size and field str modifier
BYTE* getRawField(SDDS *sdds, char *fieldName, uint32_t *fieldSize, BYTE *fieldStrModifier)
{
	if (fieldName && sdds)
	{
		for (uint64_t i = 0; i < sdds->FieldCount; i++)
		{
			if (strcmp(fieldName, sdds->FieldNames[i]) == 0)
			{
				if (fieldSize)
				{
					*fieldSize = sdds->FieldSizes[i];
				}
				if (fieldStrModifier)
				{
					*fieldStrModifier = sdds->FieldStrModifiers[i];
				}
				return sdds->Fields[i];
			}
		}
	}
	return NULL;
}

// Adds field to the SDDS
bool addField(SDDS *sdds, char* fieldName, uint32_t fieldSize, BYTE* rawField, BYTE fieldStrModifier)
{
	initialize(sdds);

	// Make sure the new fieldName is unique
	if (getRawField(sdds, fieldName, NULL, NULL))
	{
		// Name conflict, name already exists.
		return false;
	}

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

	// Realloc the Fields and FieldNames
	if (!(reallocPPPByte(&sdds->Fields, sdds->FieldCount + 1, copiedRawField) && \
		reallocPPPByte(((BYTE***)&sdds->FieldNames), sdds->FieldCount + 1, (BYTE*)copiedFieldName)))
	{
		// free already allocated
		free(copiedFieldName);
		free(copiedRawField);
		return false;
	}

	// Only set and increment the FieldCount if everything went well.
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
		stringAppend(&retStr, ">");

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
// gcc -Wall -pedantic Source.c Memory.c -std=c99 -lm && ./a.out && rm a.out


// Overall Todos:
/*
- Better split up SDDS files into headers/implementation files maybe even forward declare.
- Ability to remove fields by name
- Implement usage of FieldStrModifiers, and make toString() use it.
- Add way to go 'toBytes' and get a native byte-buffer representation of just the data (without names, etc)
- Add way to create from xml
- Add support for nesting

--> Then we have -> Decent parity with the struct functionality and serialization!
*/