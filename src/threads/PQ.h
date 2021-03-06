#ifndef THREADS_PQ_H
#define THREADS_PQ_H

#include <stdio.h>
//#include <stdlib.h>
#include "malloc.h"

typedef struct PQNode_tag {
	int64_t priority;
	void * ptr;
	struct PQNode_tag * next;
} PQNode;

typedef struct PQHead_tag {
	struct PQNode_tag * head;
} PQHead;

extern void createPQHead(PQHead * pqh);
extern void pushPQ(PQHead * pqh, void * pt, int64_t pr);
extern void * popPQ(PQHead * pqh);
extern void * peekPQ(PQHead * pqh);

#endif /* threads/PQ.h */
