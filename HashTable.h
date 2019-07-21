#pragma once

typedef struct HashTable Node;

struct HashTable
{
	char* Key;
	int Value;
	Node* pNext;
};
