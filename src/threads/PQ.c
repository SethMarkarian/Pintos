#include "PQ.h"

extern void createPQHead(PQHead * pqh){
	pqh -> head = NULL;
}

extern void pushPQ(PQHead * pqh, void * pt, int64_t pr){
	PQNode * ins = malloc(sizeof(PQNode));
	ins -> ptr = pt;
	ins -> priority = pr;
	ins -> next = NULL;
	PQNode * prev = pqh -> head;
	if(prev == NULL){ // empty
		pqh -> head = ins;
		return;
	}
	if(prev -> priority > pr){ // insert at front of queue
		ins -> next = prev;
		pqh -> head = ins;
		return;
	}
	PQNode * nxt = prev -> next;
	while(nxt != NULL){ // locate appropriate insert location
		if (nxt -> priority > pr) break; // specific to Pintos
		prev = nxt;
		nxt = nxt -> next;
	}
	// link everything up
	prev -> next = ins;
	ins -> next = nxt;
	return;
}

extern void * popPQ(PQHead * pqh){
	if(pqh -> head == NULL) return NULL;
	void * ret = pqh -> head -> ptr;
	PQNode * next = pqh -> head -> next;
	free(pqh -> head);
	pqh -> head = next;
	return ret;
}

extern void * peekPQ(PQHead * pqh){
	if(pqh -> head == NULL) return NULL;
	return pqh -> head -> ptr;
}
