#include <DList.h>
#include <dbg.h>

DList* DList_create()
{
	return calloc(1, sizeof(DList));
}

void DList_destroy(DList* DList)
{
	DList_FOREACH(DList, first, next, cur){
		if(cur->prev){
			free(cur->prev);
		}
	}
	
	free(DList->last);
	free(DList);
}

void DList_clear(DList* DList)
{
	DList_FOREACH(DList, first, next, cur){
		free(cur->value);
	}
}

void DList_clear_destroy(DList* DList)
{
	DList_clear(DList);
	DList_destroy(DList);
}

void DList_push(DList* DList, void* value)
{
	DListNode* node = calloc(1, sizeof(DListNode));
	check_mem(node);
	
	node->value = value;
	
	if(DList->last == NULL){
		DList->first = node;
		DList->last = node;
	}else{
		DList->last->next = node;
		node->prev = DList->last;
		DList->last = node;
	}
	
	DList->count++;
	
	error:
		return;
}

void* DList_pop(DList* DList)
{
	DListNode* node = DList->last;
	return node != NULL ? DList_remove(DList, node) : NULL;
}

void DList_unshift(DList* DList, void* value)
{
	DListNode* node = calloc(1, sizeof(DListNode));
	check_mem(node);
	
	node->value = value;
	
	if(DList->first == NULL){
		DList->first = node;
		DList->last = node;
	}else{
		node->next = DList->first;
		DList->first->prev = node;
		DList->first = node;
	}
	
	DList->count++;
	
	error:
		return;
}

void* DList_shift(DList* DList)
{
	DListNode* node = DList->first;
	return node != NULL ? DList_remove(DList, node) : NULL;
}

void* DList_remove(DList* DList, DListNode* node)
{
	void* result = NULL;
	
	check(DList->first && DList->last, "DList is empty.");
	check(node, "node can't be NULL");
	
	if(node == DList->first && node == DList->last){
		DList->first = NULL;
		DList->last = NULL;
	}else if(node == DList->first){
		DList->first = node->next;
		check(DList->first != NULL, "Invalid DList, somehow got a first that is NULL.");
		DList->first->prev = NULL;
	}else if(node == DList->last){
		DList->last = node->prev;
		check(DList->last != NULL, "Invalid DList, somehow got a next that is NULL.");
		DList->last->next = NULL;
	}else{
		DListNode* after = node->next;
		DListNode* before = node->prev;
		after->prev = before;
		before->next = after;
	}
	
	DList->count--;
	result = node->value;
	free(node);
	
	error:
		return result;
}