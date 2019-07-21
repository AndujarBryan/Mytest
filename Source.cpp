#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include "Hashing.h"



using namespace std;

#define FILENAME_LEN 50
#define FILENAME_NUM 100
#define HASH_TABLE_MAX_SIZE 100000

//char text[FILENAME_NUM][MAX_LINE_SIZE * 50];
//int hash_size[FILENAME_NUM];

int main()
{
	Hashing DOE;
	int file_num = 0;
	FILE *read_filename = fopen("filename_list.txt", "r");
	char **filename_list_array = (char **)malloc(sizeof(char*)* FILENAME_NUM);
	int i, j; 
	typedef struct HashTable Node;
	Node* pHead;

	Node* hashTable[FILENAME_NUM + 1][HASH_TABLE_MAX_SIZE];

#pragma omp parallel for
	for (i = 0; i < FILENAME_NUM; i++)
	{
		filename_list_array[i] = (char *)malloc(sizeof(char)* FILENAME_LEN);
	}

	if (read_filename == NULL)
	{
		cout << "open the file incorrectly !";
		return 0;
	}
	while (!feof(read_filename))
	{
		fscanf(read_filename, "%s\n", filename_list_array[file_num]);
		file_num++;
	}
	cout << "The result can be found in output.txt .";

	omp_set_num_threads(8);
	double time = -omp_get_wtime();

#pragma omp parallel private (i)
	{
#pragma omp single nowait
		{
			for (i = 0; i < file_num; i++)
			{
#pragma omp task
				{
					DOE.readFunc(filename_list_array[i], i);
				}
			}
		}
	}
#pragma omp parallel private (i)
	{
#pragma omp single nowait
		{
			for (i = 0; i < file_num; i++)
			{
#pragma omp task 
				{
					DOE.mapperFunc(i);
				}
			}
		}
	}
#pragma omp barrier
	DOE.reducerFunc(file_num);
#pragma omp barrier
	FILE *fp = fopen("output.txt", "w");

	DOE.writerFunc(fp);

	DOE.hashTablePrintFunc(FILENAME_NUM);

#pragma omp parallel for
	for (i = 0; i < FILENAME_NUM; i++)
	{
		for (j = 0; j < HASH_TABLE_MAX_SIZE; j++)
		{
			if (hashTable[i][j])
			{
				pHead = hashTable[i][j];
				if (pHead)
				{
					free(pHead->Key);
					free(pHead);
				}
			}
		}
	}
	fclose(fp);
	time = time + omp_get_wtime();
	printf("Elapsed time is %lf seconds. \n", time);

	for (i = 0; i < FILENAME_NUM; i++)
	{
		free(filename_list_array[i]);
	}
	free(filename_list_array);

	return 0;
}


