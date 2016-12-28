#include "sorted-list.h"
#include <stdio.h>

SortedListPtr SLCreate(CompareFuncT cf, DestructFuncT df) 
{

	SortedListPtr slp = (SortedListPtr) malloc(sizeof(struct SortedList));

	if(!slp)
		return NULL;

	slp -> head = NULL;
	slp -> modifications = 0;
	slp -> cf = cf;
	sl p-> df = df;

	return slp;

}


void SLDestroy(SortedListPtr list) 
{

	if(list)
	{

		Node node = list -> head;


		while(node)
		{

			Node next = node -> next;
			list -> df(node -> value);
			free(node);
			node = next;

		}

		free(list);

	}

}


int SLInsert(SortedListPtr list, void *newObj) 
{

	if(list)
	{

		Node node = list -> head;
		list -> modifications ++;


		if(!node)
		{

			Node new = (Node) malloc(sizeof(struct Node));
			new -> next = NULL;
			new -> value = newObj;
			list -> head = new;

			return 1;

		}


		if(list -> cf(node->value, newObj) <= 0)
		{

			Node new = (Node) malloc(sizeof(struct Node));
			new -> next = list -> head;
			new -> value = newObj;
			list -> head = new;
			list -> modifications++;

			return 1;

		}


		while(node -> next && list -> cf(node -> next -> value, newObj) == 1)
		{

			node = node -> next;

		}


		Node new = (Node) malloc(sizeof(struct Node));
		new -> next = node -> next;
		new -> value = newObj;
		node -> next = new;

		return 1;

	}

	return 0;

}




int SLRemove(SortedListPtr list, void *newObj) {

	if(list)
	{

		Node node = list -> head;


		if (!node)
		{
			return 0;
		}

	
		if (list -> cf(node -> value, newObj) == 0) 
		{

			list -> modifications ++;
			list -> head = node -> next;
			free(node);

			return 1;

		}


		while (node -> next) 
		{

			if (list -> cf(node -> next -> value, newObj) == 0)
			{

				Node temp = node -> next;
				node -> next = node -> next -> next;
				free(temp);
				list -> modifications ++;

				return 1;

			}

			node = node -> next;

		}

		return 0;

	}

	return 0;

}


SortedListIteratorPtr SLCreateIterator(SortedListPtr list)
{

	if(!list)

		return NULL;

	SortedListIteratorPtr iterator = (SortedListIteratorPtr) malloc(sizeof(*iterator));


	if(!iterator)

		return NULL;


	iterator -> modifications = list -> modifications;
	iterator -> list = list;
	iterator -> current = list -> head;

	return iterator;

}


void SLDestroyIterator(SortedListIteratorPtr iter)
{

	if(iter)
		free(iter);

}


void * SLGetItem(SortedListIteratorPtr iter)
{

	if(iter && iter -> list) 
	{
		if(iter -> list -> modifications == iter -> modifications)
		{
			if(iter -> current)

				return iter -> current -> value;

		} else {

			printf("Error: Concurrrent Modification Detected. Aborted.");

			abort();

		}

	}

	return NULL;

}


void * SLNextItem(SortedListIteratorPtr iter)
{

	if(iter && iter -> list)
	{
		if(iter -> list -> modifications == iter -> modifications)
		{
			if(iter -> current)
				iter -> current = iter -> current -> next;

			return SLGetItem(iter);

		} else {

			printf("Error: Concurrent Modification Detected. Aborted.");

			abort();

		}

	} else {

		return NULL;

	}

}

