#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Hashing.h"
#include <omp.h>
#include "HashTable.h"



using namespace std;

//#define FILENAME_LEN 50
#define FILENAME_NUM 100
#define HASH_TABLE_MAX_SIZE 100000
#define MAX_LINE_SIZE 100000

char text[FILENAME_NUM][MAX_LINE_SIZE * 50];
int hash_size[FILENAME_NUM];

Node* hashTable[FILENAME_NUM + 1][HASH_TABLE_MAX_SIZE];

Hashing::Hashing()
{
}

Hashing::~Hashing()
{
}

unsigned int Hashing::hashComputeByHashFunc(const char * key) //dont know its function yet
{
	unsigned int hash = 1;
	int temp;
	while (temp = *key++)
	{
		hash = hash * 33 + temp;
	}
	return hash;
}

void Hashing::hashTableInsertFunc(const char * key, int value, int index) //IDK
{
	unsigned int pos, poscheck;
	Node* NewNode = (Node*)malloc(sizeof(Node));
	memset(NewNode, 0, sizeof(Node));
	NewNode->Key = (char*)malloc(sizeof(char)* (strlen(key) + 1));
	strcpy(NewNode->Key, key);
	NewNode->Value = value;

	pos = hashComputeByHashFunc(key) % HASH_TABLE_MAX_SIZE;
	poscheck = pos;

	while (hashTable[index][poscheck] != NULL && hashTable[index][poscheck]->Value != -1)
	{
		poscheck++;
		poscheck %= HASH_TABLE_MAX_SIZE;
	}
	NewNode->pNext = hashTable[index][pos];
	hashTable[index][pos] = NewNode;

	hash_size[index]++;
}

Node * Hashing::hashTableLookupFunc(const char * key, int index)
{
	unsigned int pos = hashComputeByHashFunc(key) % HASH_TABLE_MAX_SIZE;
	Node* pHead;
	if (hashTable[index][pos])
	{
		pHead = hashTable[index][pos];
		while (pHead)
		{
			if (strcmp(key, pHead->Key) == 0)
			{
				return pHead;
			}
			pHead = pHead->pNext;
		}
	}
	return NULL;
}

void Hashing::hashTablePrintFunc(int index)
{
	int i = 0;
	Node* pHead;
	printf("=========== content of hash table ===========\n");
	while (i < HASH_TABLE_MAX_SIZE)
	{
		if (hashTable[index][i])
		{
			pHead = hashTable[index][i];
			while (pHead)
			{
				printf("Word: %s, count: %d ", pHead->Key, pHead->Value);
				pHead = pHead->pNext;
				printf("\n");
			}
		}
		i++;
	}
}

void Hashing::rmPunct2LowerFunc(char * p)
{
	char *src = p, *dst = p;
	while (*src)
	{
		if (ispunct((unsigned char)*src) || isdigit((unsigned char)*src))
		{
			src++;
		}
		else if (isupper((unsigned char)*src))
		{
			*dst++ = tolower((unsigned char)*src);
			src++;
		}
		else if (src == dst)
		{
			src++;
			dst++;
		}
		else
		{
			*dst++ = *src++;
		}
	}
	*dst = 0;
}

void Hashing::readFunc(const char * filename, int index)
{
	FILE *fp = fopen(filename, "r");
	char word[1000];
	if (fp == NULL)
	{
		printf("input file is invalid !");
		return;
	}

	while (fscanf(fp, " %s", word) == 1)
	{
		rmPunct2LowerFunc(word);
		strcat(text[index], word);
		strcat(text[index], " ");
	}
	fclose(fp);
}

void Hashing::mapperFunc(int index)
{
	char *nextWord;
	nextWord = strtok(text[index], " \r\n");
	while (nextWord != NULL)
	{
#pragma omp critical 
		{
			if (hashTableLookupFunc(nextWord, index) == NULL)
			{
				hashTableInsertFunc(nextWord, 1, index);
			}
			else
			{
				hashTableLookupFunc(nextWord, index)->Value++;
			}
			nextWord = strtok(NULL, " \r\n");
		}
	}
}

void Hashing::reducerFunc(int reduceCount)
{
	int i, j;
#pragma omp parallel for
	for (i = 0; i < reduceCount; ++i)
	{
		for (j = 0; j < HASH_TABLE_MAX_SIZE; j++)
		{
#pragma omp critical
			if (hashTable[i][j])
			{
				Node* pHead = hashTable[i][j];
				while (pHead)
				{
					if (hashTableLookupFunc(pHead->Key, FILENAME_NUM) == NULL)
						hashTableInsertFunc(pHead->Key, pHead->Value, FILENAME_NUM);
					else
					{
						int val = pHead->Value;
						hashTableLookupFunc(pHead->Key, FILENAME_NUM)->Value += val;
					}
					pHead = pHead->pNext;
				}
			}
		}
	}
}

void Hashing::writerFunc(FILE * fp)
{
	int i;
	Node* p;
	fprintf(fp, "------print the result------ \n");
#pragma omp critical 
	{
		for (i = 0; i < HASH_TABLE_MAX_SIZE; ++i)
		{
			if (hashTable[FILENAME_NUM][i] != NULL)
			{
				p = hashTable[FILENAME_NUM][i];
				while (p)
				{
					fprintf(fp, "Word: %s, Count: %d\n", p->Key, p->Value);
					p = p->pNext;
				}
			}
		}
	}
}



