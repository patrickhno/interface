
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdlib.h>
#include <posix/stdio.h>
//#include "Debug.h"

//#include <memory.h>
#include <assert.h>
#include "DynamicArray.h"
//#include "rme_assert.h"
#include "rme_Memory.h"
#include <posix/string.h>
#include <Interface/Semaphore.h>

DynamicArray::DynamicArray(){
	Count=0;
	BufferCount=0;
	Array=0;
	IDArray=0;
	flags = 0;
//	sem = new Semaphore();
//	#ifdef _DEBUG
//	threadID = GetCurrentThreadId();
//	#endif
}

DynamicArray::DynamicArray(int f){
	Count=0;
	BufferCount=0;
	Array=0;
	IDArray=0;
	flags = f;
//	sem = new Semaphore();
//	#ifdef _DEBUG
//	threadID = GetCurrentThreadId();
//	#endif
}

#ifdef _DEBUG
DynamicArray::DynamicArray(DynamicArray *a, char *file, int size){
#else
DynamicArray::DynamicArray(DynamicArray *a){
#endif
//		int			Count;
//		int			BufferCount;
//		int			flags;
//		void		**Array;
//		int			*IDArray;

	flags = a->flags;
	Count = a->Count;
	BufferCount=a->BufferCount;
	#ifdef _DEBUG
	if(flags&DARRAY_SHARED_MEMORY) Array=(void**)dbAllocate(sizeof(void*)*BufferCount,MEM_SHARED,file,size);
	#else
	if(flags&DARRAY_SHARED_MEMORY) Array=(void**)Allocate(sizeof(void*)*BufferCount,MEM_SHARED);
	#endif
	else Array=(void**)malloc(sizeof(void*)*BufferCount);
	memcpy(Array,a->Array,sizeof(void*)*BufferCount);
	if(flags&DARRAY_GENID){
		IDArray=(int*)malloc(sizeof(int)*BufferCount);
		memcpy(IDArray,a->IDArray,sizeof(int)*BufferCount);
	}else{
		IDArray=0;
	}
//	#ifdef _DEBUG
//	threadID = GetCurrentThreadId();
//	#endif
}
DynamicArray::~DynamicArray(){
//	assert(threadID!=GetCurrentThreadId());
//printf("A\n");
	if(flags&DARRAY_SHARED_MEMORY){
//printf("B\n");
		if(Array) Free(Array);
//printf("C\n");
		if(IDArray)	Free(IDArray);
//printf("D\n");
	}else{
//printf("E\n");
		if(Array) free(Array);
//printf("F\n");
		if(IDArray)	free(IDArray);
//printf("G\n");
	}
//printf("H\n");
//	delete sem;
}

void DynamicArray::New(void){
//	assert(threadID!=GetCurrentThreadId());
	if(!(flags&DARRAY_NOSHRINK)){
		if(flags&DARRAY_SHARED_MEMORY){
			if(Array) Free(Array);
			if(IDArray)	Free(IDArray);
		}else{
			if(Array) free(Array);
			if(IDArray)	free(IDArray);
		}
		Array=0;
		IDArray=0;
		BufferCount=0;
	}
	Count=0;
}

//int DynamicArray::GetCount(void){
//	return(Count);
//}

#ifdef _DEBUG
int DynamicArray::Add(void *item, char *file, int size){	// fixme: this function should return failure if realloc fails
#else
int DynamicArray::Add(void *item){	// fixme: this function should return failure if realloc fails
#endif
//	assert(threadID!=GetCurrentThreadId());
//sem->Acquire();
	if(flags&DARRAY_GENID){
		// Create an unique ID for this item
	seekID:
		int ID=rand()*Count+1;
		for(int n=0; n<Count; n++){
			//if(Array[n]==item){
				if(IDArray[n]==ID) goto seekID;
			//}
		}
		//
		Count++;
		if(flags&DARRAY_SHARED_MEMORY){
			#ifdef _DEBUG
			if(Array) Array=(void**)dbReAllocate(Array,sizeof(void*)*Count,file,size);
			else Array=(void**)dbAllocate(sizeof(void*)*Count,MEM_SHARED,file,size);
			if(IDArray) IDArray=(int*)dbReAllocate(IDArray,sizeof(int)*Count,file,size);
			else IDArray=(int*)dbAllocate(sizeof(int)*Count,MEM_SHARED,file,size);
			#else
			if(Array) Array=(void**)ReAllocate(Array,sizeof(void*)*Count);
			else Array=(void**)Allocate(sizeof(void*)*Count,MEM_SHARED);
			if(IDArray) IDArray=(int*)ReAllocate(IDArray,sizeof(int)*Count);
			else IDArray=(int*)Allocate(sizeof(int)*Count,MEM_SHARED);
			#endif
		}else{
			Array=(void**)realloc(Array,sizeof(void*)*Count);
			IDArray=(int*)realloc(IDArray,sizeof(int)*Count);
		}
		Array[Count-1]=item;
		IDArray[Count-1]=ID;
//sem->Release();
		return ID;
	}else{
		//
		Count++;
		if(Count<=BufferCount){
			Array[Count-1]=item;
		}else{
			if(flags&DARRAY_SHARED_MEMORY){
				#ifdef _DEBUG
				if(Array) Array=(void**)dbReAllocate(Array,sizeof(void*)*Count,file,size);
				else Array=(void**)dbAllocate(sizeof(void*)*Count,MEM_SHARED,file,size);
				#else
				if(Array) Array=(void**)ReAllocate(Array,sizeof(void*)*Count);
				else Array=(void**)Allocate(sizeof(void*)*Count,MEM_SHARED);
				#endif
			}
			else{
//printf("%x,%d\n",this,sizeof(void*)*Count);
				Array=(void**)realloc(Array,sizeof(void*)*Count);
			}
			//IDArray=(int*)realloc(IDArray,sizeof(int)*Count);
			Array[Count-1]=item;
			BufferCount=Count;
		}
//for(int n=1; n<Count; n++){
//	if(Array[n-1]==item) assert(true);
//}
//sem->Release();
		return 0;
	}
}

#ifdef _DEBUG
void DynamicArray::Remove(void *item, int ID, char *file, int size){
#else
void DynamicArray::Remove(void *item, int ID){
#endif
//	assert(threadID!=GetCurrentThreadId());
	assert(!(flags&DARRAY_NOSHRINK));
	// find match
	for(int n=0; n<Count; n++){
		if((Array[n]==item)&&(IDArray[n]==ID)){
			// Remove
			void **buf;
			int *IDbuf;
			if(flags&DARRAY_SHARED_MEMORY){
				if(Count>1){
					#ifdef _DEBUG
					buf=(void**)dbAllocate(sizeof(void*)*(Count-1),MEM_SHARED,file,size);
					IDbuf=(int*)dbAllocate(sizeof(int)*(Count-1),MEM_SHARED,file,size);
					#else
					buf=(void**)Allocate(sizeof(void*)*(Count-1),MEM_SHARED);
					IDbuf=(int*)Allocate(sizeof(int)*(Count-1),MEM_SHARED);
					#endif
				}else{
					buf = 0;
					IDbuf = 0;
				}
			}else{
				if(Count>1){ //flags&DARRAY_SHARED_MEMORY){	// fixme: malloc crash
					buf=(void**)malloc(sizeof(void*)*(Count-1));
					IDbuf=(int*)malloc(sizeof(int)*(Count-1));
				}else{
					buf = 0;
					IDbuf = 0;
				}
			}
			void **b1=buf;
			int *b2=IDbuf;
			for(int c=0; c<n; c++){
				*b1++=Array[c];
				*b2++=IDArray[c];
			}
			for(c=n+1; c<Count; c++){
				*b1++=Array[c];
				*b2++=IDArray[c];
			}
			if(flags&DARRAY_SHARED_MEMORY){
				Free(Array);
				Free(IDArray);
			}else{
				free(Array);
				free(IDArray);
			}
			Array=buf;
			IDArray=IDbuf;
			Count--;
			BufferCount--;
		}
	}
}

#ifdef _DEBUG
void DynamicArray::Remove(int index, char *file, int size){
#else
void DynamicArray::Remove(int index){
#endif
//	assert(threadID!=GetCurrentThreadId());
	assert(!(flags&DARRAY_NOSHRINK));
	// Remove
	if(IDArray){
		void **buf;
		int *IDbuf;
		if(Count>1){	// fixme: malloc/free bug
			buf=(void**)malloc(sizeof(void*)*(Count-1));
			IDbuf=(int*)malloc(sizeof(int)*(Count-1));
		}else{
			buf = 0;
			IDbuf = 0;
		}
		void **b1=buf;
		int *b2=IDbuf;
		for(int c=0; c<index; c++){
			*b1++=Array[c];
			*b2++=IDArray[c];
		}
		for(c=index+1; c<Count; c++){
			*b1++=Array[c];
			*b2++=IDArray[c];
		}
		free(Array);
		free(IDArray);
		Array=buf;
		IDArray=IDbuf;
		Count--;
		BufferCount--;
	}else{
		void **buf;
		if(flags&DARRAY_SHARED_MEMORY){
			#ifdef _DEBUG
			if(Count>1) buf = (void**)dbAllocate(sizeof(void*)*(Count-1),MEM_SHARED,file,size);
			#else
			if(Count>1) buf = (void**)Allocate(sizeof(void*)*(Count-1),MEM_SHARED);
			#endif
			else buf = 0;
		}
		else buf = (void**)malloc(sizeof(void*)*(Count-1));
		void **b1=buf;
		for(int c=0; c<index; c++){
			*b1++=Array[c];
		}
		for(c=index+1; c<Count; c++){
			*b1++=Array[c];
		}
		if(flags&DARRAY_SHARED_MEMORY) Free(Array);
		else free(Array);
		Array=buf;
		Count--;
		BufferCount--;
	}
}

void DynamicArray::Clear(void){
//	assert(threadID!=GetCurrentThreadId());
//sem->Acquire();
	if(!(flags&DARRAY_NOSHRINK)){
		if(Array){
			if(flags&DARRAY_SHARED_MEMORY) Free(Array);
			else free(Array);
		}
		if(IDArray)	free(IDArray);
		Array=0;
		IDArray=0;
		BufferCount=0;
	}
	Count=0;
//sem->Release();
}
/*
void *DynamicArray::GetItem(int n){
//sem->Acquire();
//	void *foo = Array[n];
//sem->Release();
	return(Array[n]);
}
*/
void *DynamicArray::GetBuffer(void){
//	assert(threadID!=GetCurrentThreadId());
	return(Array);
}

int DynamicArray::GetItemID(int n){
//	assert(threadID!=GetCurrentThreadId());
	return(IDArray[n]);
}

void DynamicArray::Sort(void){
//	assert(threadID!=GetCurrentThreadId());
	bool redo;
	do{
		redo=false;
		for(int n=0; n<(Count-1); n++){
			if(Swap(Array[n],Array[n+1])){
				void *tmp = Array[n];
				Array[n]=Array[n+1];
				Array[n+1]=tmp;
				if(IDArray){
					int tmp2 = IDArray[n];
					IDArray[n]=IDArray[n+1];
					IDArray[n+1]=tmp2;
				}
				redo=true;
			}
		}
	}while(redo);
}
/*
bool DynamicArray::DoSwap(void *ent1, void *ent2){
	if(ent2<ent1) return true;
	else return false;
}
*/

bool DynamicArray::Swap(void *a, void *b){
	return(a>b);
}

