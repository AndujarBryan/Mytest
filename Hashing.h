#pragma once
#include "HashTable.h"

class Hashing
{
public:
	Hashing();
	~Hashing();
	Node* pHead;
	unsigned int hashComputeByHashFunc(const char* key);
	void hashTableInsertFunc(const char* key, int value, int index);
	Node* hashTableLookupFunc(const char* key, int index);
	void hashTablePrintFunc(int index);
	void rmPunct2LowerFunc(char * p);
	void readFunc(const char* filename, int index);
	void mapperFunc(int index);
	void reducerFunc(int reduceCount);
	void writerFunc(FILE * fp);

};