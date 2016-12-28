#include <stdio.h>
#include <assert.h>
#include "var.h"
#include <stdlib.h>
#include "linkedlist.h"

linkedList newLinkedList(void* data) 
{
	linkedList l = (linkedList) malloc(sizeof(*l));
	l -> head = l -> tail = NULL;
	linkedNode n;


	if(data)
	{

		assert(data);
		n = newLinkedNode(data);
		l -> head = l -> tail = n;

	}

	return l;

}



linkedNode newLinkedNode(void* data)
{

	assert(data);

	linkedNode n = (linkedNode) malloc(sizeof(*n));
	n -> content = data;
	n -> next = NULL;

	return n;

}


bool linkedListIsEmpty(linkedList l) 
{
	assert(l);
	return l -> head == NULL && l -> tail == NULL;

}


void appendLinkedNode(linkedNode n, linkedList l)
{

	assert(l);
	assert(n);

	if(!l -> tail)

		l -> head = l -> tail = n;

	else

		l -> tail = l -> tail -> next = n;

}



linkedNode removeLinkedNode(void* data, linkedList l)
{ 

	assert(l);
	assert(l -> head);

	if(l -> head -> content == data)
	{

		if(l -> tail == l -> head)
			l -> tail = NULL;

		linkedNode n = l -> head;
		l -> head = l -> head -> next;

		return n;

	}

	linkedNode n = l -> head;

	while(n -> next)
	{
		if(n -> next -> content == data){

			linkedNode ret = n -> next;

			if(n -> next == l -> tail)

				l -> tail = n;

				n -> next = n -> next -> next;


			return ret;

		}

	}

	return NULL;

}



linkedNode removeLinkedListHead(linkedList l)
{

	assert(l);
	assert(l -> head);
	linkedNode n;

	if(l -> tail == l -> head)

		l -> tail = NULL;
		n = l -> head;
		l -> head = n -> next;

	return n;

}

void printLinkedList(linkedList l, void (*printFunction)(void*))
{

	if(l) 
	{

		printf("Printing linked list\n");
		linkedNode current = l -> head;


		while(current)
		{

			printFunction(current -> content);
			current = current -> next;

		}

	}

	printf("\n");

}

void deleteLinkedList(linkedList l, void (*deleteFunction)(void*))
{

	if(l)
	{
		if(l -> head)
	{

			linkedNode node = l->head;

			while(node)
			{

				linkedNode temp = node;
				node = temp -> next;
				deleteFunction(temp -> content);
				free(temp);

			}

		}

		free(l);

	}

}


