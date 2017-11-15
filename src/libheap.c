#include <stdlib.h>
#include <libheap.h>
#include <lib8080.h>
#include <libemulator.h>

extern int MemUsed;
int HeapSize = 0;

char *HeapAddProcess(Process *p){

	Page *ProcessCursor 	= (Page*) Processes;
	Process *PtrCursor	= (Process *)(ProcessCursor + 1);
	
}

Process **HeapGetNode(int Node){
	Page *PageCursor = (Page *)Processes;
	for(int i = 0; i < Node/(4096/sizeof(Process *)); i++){
		PageCursor = (Page *)PageCursor->Next;
		if(PageCursor == NULL)
			return NULL;
	}
	int offset = Node % (4096/sizeof(Process *));
	Process **ProcessCursor = (Process **)(PageCursor + 1) + offset;
	return ProcessCursor;
}

void HeapInsertNode(Process *p){
	Page *PageCursor = (Page *)Processes;
	Process **ProcessCursor = (Process **)(PageCursor + 1);
	int allocd = 0;
	while(1){
		for(int i = 0; i < 4096/sizeof(Process **); i++){		
			if(ProcessCursor[i] == NULL){
				HeapSize++;
				ProcessCursor[i] = p;
				//Don't hate me cause I'm beautiful
				goto out;
			}
		}
		if(PageCursor->Next == NULL){
			PageCursor->Next = PageAlloc();
			printf("%p - %p\n", PageCursor->Next, MEM_LIMIT);
		}
		PageCursor = (Page *)(PageCursor->Next);
		ProcessCursor = (Process **)(PageCursor + 1);
	}
	out: HeapBubbleFrom(ProcessCursor, HeapSize - 1);
}

void MaxHeapify(Page *Head, int Nodes, int Index){
	int Left = Index + Index + 1;
	int Right = Index + (Index + 2);
	int Largest = (Index);

	Process **RootNode  = HeapGetNode(Index);
	Process **LeftNode  = HeapGetNode(Left);
	Process **RightNode = HeapGetNode(Right);

	Process **LargestNode = RootNode;
	if(Left <= Nodes && (*LeftNode)->priority > (*RootNode)->priority){
		Largest = Left;
		LargestNode = LeftNode;
	}

	if(Right <= Nodes && (*RightNode)->priority > (*LargestNode)->priority){
		Largest = Right;
		LargestNode = RightNode;
	}

	if(LargestNode != RootNode){
		printf("\t swapping %d @ %d and %d @ %d\n", (*RootNode)->priority, Index, (*LargestNode)->priority, Largest);
		Process *tmp = *RootNode;
		*RootNode = *LargestNode;
		*LargestNode = tmp;
		MaxHeapify(Head, Nodes, Largest);
	}
}

void HeapIncreaseKey(Page *Head, int Nodes, int Index, int Key){
	Process **P = HeapGetNode(Index);
	if((*P)->priority > Key){
		return;
	}
	(*P)->priority = Key;

	HeapBubbleFrom(P, Index);
/*	Process **Parent = HeapGetNode(Index/2);
	while(Index > 1 && (*Parent)->priority < (*P)->priority){

		Process *tmp = *P;
		*P = *Parent;
		*Parent = tmp;
	}*/
}

void HeapBubbleFrom(Process **Head, int Index){
	Process **Parent = HeapGetNode(Index/2);
	while(Index > 1 && (*Parent)->priority < (*Head)->priority){
		Process *tmp = *Head;
		*Head = *Parent;
		*Parent = tmp;
		Index /= 2;
	}
}
