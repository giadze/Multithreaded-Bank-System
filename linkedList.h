#include "types.h"
#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef struct linkedList* linkedList;
typedef struct linkedNode* linkedNode;

struct linkedList {

	linkedNode head;
	linkedNode tail;

};


struct linkedNode {

	void* content;
	linkedNode next;

};


bool linkedListIsEmpty(linkedList l);
linkedList newLinkedList(void* content);
linkedNode newLinkedNode(void* content);
void appendLinkedNode(linkedNode n, linkedList l);
linkedNode removeLinkedListHead(linkedList l);
linkedNode removeLinkedNode(void* content, linkedList l);
void deleteLinkedList(linkedList l, void (*deleteFunction)(void*));
void printLinkedList(linkedList l, void (*printFunction)(void*));

#endif

