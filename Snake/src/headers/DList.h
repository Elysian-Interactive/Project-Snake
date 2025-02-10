#ifndef __DLIST_H__
#define __DLIST_H__

#include <stdlib.h>

struct DListNode;

typedef struct DListNode{
	struct DListNode* next;
	struct DListNode* prev;
	void* value;
}DListNode;

typedef struct DList{
	int count;
	DListNode* first;
	DListNode* last;
}DList;

DList* DList_create();
void DList_destroy(DList* DList);
void DList_clear(DList* DList);
void DList_clear_destroy(DList* DList);

#define DList_count(A) ((A)->count)
#define DList_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
#define DList_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

void DList_push(DList* DList, void* value);
void* DList_pop(DList* DList);
void DList_unshift(DList* DList, void* value);
void* DList_shift(DList* DList);

void* DList_remove(DList* DList, DListNode* value);

#define DList_FOREACH(L, S, M, V) DListNode* _node = NULL; DListNode* V = NULL;\
for(V = _node = L->S; _node != NULL; V = _node = _node->M)

#endif