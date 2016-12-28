#ifndef SORTED_LIST_H
#define SORTED_LIST_H
#include <stdlib.h>

typedef int (*CompareFuncT)( void *, void * );
typedef void (*DestructFuncT)( void * );

struct Node 
{

	void* value;
	struct Node* next;

};

typedef struct Node* Node;


struct SortedList

{

	Node head;
	int modifications;
	CompareFuncT cf;
	DestructFuncT df;

};

typedef struct SortedList* SortedListPtr;


struct SortedListIterator
{

	SortedListPtr list;
	Node current;
	int modifications;

};

typedef struct SortedListIterator* SortedListIteratorPtr;
void SLDestroy(SortedListPtr list);
int SLInsert(SortedListPtr list, void *newObj);
int SLRemove(SortedListPtr list, void *newObj);

SortedListIteratorPtr SLCreateIterator(SortedListPtr list);

void SLDestroyIterator(SortedListIteratorPtr iter);
void * SLGetItem( SortedListIteratorPtr iter );
void * SLNextItem(SortedListIteratorPtr iter);

#endif

