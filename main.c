// Eliot Martin - Cache simulator

// usage:
// w (write): pick hex address (ex 0xaabb)
// pick a value (ex 0x0)
//
// p (print) : (prints cache)
//
// r (read): read hex address (0xddcc)

#include <stdio.h>
#include <stdlib.h>

struct Line {
	unsigned char data[4];
	unsigned int tag;
	unsigned char valid;
};

struct Cache {
	struct Line *lines;
	int numLines;
};

unsigned int getOffset(unsigned int address) {
	// 4B blocks, so offset is bits 0-1
	return address & 0x3;
}

unsigned int getSet(unsigned int address) {
	// 16 sets, so offset is bits 2-6
	return (address >> 2) & 0xF;
}

unsigned int getTag(unsigned int address) {
	// Offset and set are 6 bits total, so tag is high-order 26 bits
	return address >> 6;
}

struct Cache* mallocCache(int numLines) {
	// malloc a pointer to a struct Cache, malloc a pointer to an array
	// of struct Line instances (array length is numLines). initialize
	// valid to 0 for each struct Line. Return the struct Cache pointer.

	// allocate space for cache
	struct Cache *ourCache = (struct Cache*)malloc(sizeof (struct Cache));
	// initialize numlines to numlines(argument)
	ourCache->numLines = numLines;
	// allocate space for array of lines (cache attribute)
	struct Line *lineArray = (struct Line*)malloc(numLines*sizeof(struct Line));
	for (int i = 0; i<numLines;i++){
		// initialize each lign valid to 0
		lineArray[i].valid = 0;
	}
	// initialize cache attribute lines to array of Line structs
	ourCache->lines = lineArray;
	// return cache
	return ourCache;
	
}

void freeCache(struct Cache *cache) {
	free(cache->lines);
	free(cache);
}

void printCache(struct Cache *cache) {
	// print all valid lines in the cache.
	
	// we can use n as a counter 
	int n = cache->numLines;
	// iterate through cache
	for(int i = 0; i < n; i++){
		// pointer to line array[i] in cache
		struct Line *line = &cache->lines[i];
		// if cache line array[i] is valid...
		if(line->valid != 0){
			// create pointer to data in line
			unsigned char *data = line->data;
			// print line 
			printf("set: %x - tag: %x - valid: %u - data: %.2x %.2x %.2x %.2x\n", i, line->tag, line->valid, data[0], data[1], data[2], data[3]);
		}
	}
}

void readValue(struct Cache *cache, unsigned int address) {
	// check the cache for a cached byte at the specified address.
	// If found, indicate a hit and print the byte. If not found, indicate
	// a miss due to either an invalid line (cold miss) or a tag mismatch
	// (conflict miss).
	
	// set of argument address
	unsigned int set = getSet(address);
	// tag of argument
	unsigned int tag = getTag(address);
	unsigned int os = getOffset(address);
	// pointer to line corresponding to set from argument address
	struct Line *line = &cache->lines[set];
	// pointer to data in set line
	unsigned char *data = line->data;
	// print set being searched for (argument)
	printf("looking for set: %x - tag: %x\n", set, tag);
	// if set has valid lines
	if(line->valid != 0){
		// print line of set
		printf("found set: %x - tag: %x - offset: %x - valid: %u - data: %.2x\n", set, line->tag, os,line->valid, data[os]);
		// do the tags match?
		if (line->tag == tag)
			printf("hit!\n");
		else
			printf("tags don't match - miss!\n");
	}
	else
		// set has no valid lines (empty)
		printf("no valid line found - miss\n");
}

void writeValue(struct Cache *cache, unsigned int address, unsigned char *newData) {
	// Calculate set and tag for address
	unsigned int s = getSet(address);
	unsigned int t = getTag(address);
	// Get pointer to cache line in the specified set
	struct Line *line = &cache->lines[s];
	// Determine if we have a valid line in the cache that does not contain the
	// specified address - we detect this by checking for a tag mismatch
	if (line->valid && line->tag != t) {
		unsigned char *data = line->data;
		printf("evicting line - set: %x - tag: %x - valid: %u - data: %.2x %.2x %.2x %.2x\n", s, line->tag, line->valid, data[0], data[1], data[2], data[3]);
	}
	// Copy new data to line (could use memcpy here instead)
	for (int i = 0; i < 4; ++i) {
		line->data[i] = newData[i];
	}
	// Update line tag, mark line as valid
	line->tag = t;
	line->valid = 1;
	printf("wrote set: %x - tag: %x - valid: %u - data: %.2x %.2x %.2x %.2x\n", s, line->tag, line->valid, newData[0], newData[1], newData[2], newData[3]);
}

int main() {
	struct Cache *cache = mallocCache(16);
	// Loop until user enters 'q'
	char c;
	do {
		printf("Enter 'r' for read, 'w' for write, 'p' to print, 'q' to quit: ");
		scanf(" %c", &c);
		if (c == 'r') {
			printf("Enter 32-bit unsigned hex address: ");
			unsigned int a;
			scanf(" %x", &a);
			readValue(cache, a);
		} else if (c == 'w') {
			printf("Enter 32-bit unsigned hex address: ");
			unsigned int a;
			scanf(" %x", &a);
			printf("Enter 32-bit unsigned hex value: ");
			unsigned int v;
			scanf(" %x", &v);
			// Get byte pointer to v
			unsigned char *data = (unsigned char *)&v;
			writeValue(cache, a, data);
		} else if (c == 'p') {
			printCache(cache);
		}
	} while (c != 'q');
	freeCache(cache);
}
