#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "var.h"
#include "linkedlist.h"
#include "hashmap.h"

map newMap(size_t size, double load)
{
	map m = malloc(sizeof(*m));
	m -> count = 0;
	m -> load = load;
	m -> size = size;
	m -> values = malloc(sizeof(linkedList) * size);
	fillEmpty(m);

	return m;

}


void delVar(void* va) 
{

	var v = (var) va;
	free(v -> content);
	free(v);

}


void deleteMap(map m) 
{

	int i;

	for (i = 0; i  <  m -> size; i ++)

	deleteLinkedList(m -> values[i], delVar);
	free(m -> values);
	free(m);

}


void printVal(void* value) { 

	var v = (var) value;
	int val = *((int*) v -> content);

	printf("%s at index %d ", v -> name, val);

}


void printMap(map m) 
{
	int i;

	printf("Size of Map: %zu\n", m -> size);


	if (m) 
	{
		for (i = 0; i < m -> size; i ++) 
		{
			if(m -> values[i])
			{
				printLinkedList(m->values[i], printVal);

			}

		}

	}

}




void insertMapValue(string key, void* value, map m)
{

	if(m -> count >= m -> size * m -> load)

		resizeMap(m, 2);


	size_t index = hashKey(key) % m -> size;


	var v = newVar(key, value);
	linkedNode node = newLinkedNode(v);
	linkedList list = m -> values[index];


	if(!list)

		list = m -> values[index] = newLinkedList(NULL);


	if(((var) (node -> content)) -> content != value)

		abort();


	appendLinkedNode(node, list);

	m -> count++;

}


void resizeMap(map m, size_t scale)
{

	linkedList* old = m -> values;
	size_t size = scale * m -> size;
	size_t oldSize = m -> size;


	m -> size = size;
	m -> values = malloc(sizeof(linkedList) * size);


	fillEmpty(m);

	size_t index;

	for(index = 0; index < oldSize; index ++)
	{

		linkedList list = old[index];


		if(list)
		{

			while(!linkedListIsEmpty(list))
			{

				linkedNode node = removeLinkedListHead(list);
				var v = node -> content;

				m -> count--; 

				insertMapValue(v -> name, v -> content, m);

				free(v);
				free(node);

			}

			free(list);

		}

	}


	free(old);

}


int hashKey(string hash_key) 
{

	int hash = 140210887;
	char* c;


	for(c = hash_key; *c; c ++)
	{

		hash *= 84567211;
		hash ^= (int) *c;
		hash %= 2147483647;

	}

	return hash;

}


void* getKeyValue(string key, map m)
{

	linkedList list = m->values[hashKey(key) % m->size];

	if(!list || !list->head)
		return NULL;

	linkedNode node = list -> head;
	var v = (var) node -> content;


	while(node && strcmp(key, v -> name))
	{

		node = node -> next;
		v = node ? (var) node -> content : NULL;

	}

	return node ? v -> content : NULL;

}


void fillEmpty(map m)
{

	size_t index;

	for(index = 0; index < m -> size; index++)

		m -> values[index] = NULL;

}

