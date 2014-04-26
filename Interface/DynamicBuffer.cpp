
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdlib.h>
#include <posix/stdio.h>
#include <posix/string.h>
//#include <memory.h>
#include "DynamicBuffer.h"
#include "DynamicArray.h"
#include "rme_memory.h"
#include <assert.h>
#include <Interface/Semaphore.h>

// fixme: That workbuffer thingie is responsible for a LOT of memory usage.

DynamicBuffer::DynamicBuffer(){
sem = new Semaphore();
sem->Acquire();
	BlockSize=32;
	BytesLeft=0;
	BufferSize=0;
	Buffer=0;
	WorkBuffer=0;
	Count=0;
	flags=0;
sem->Release();
}

DynamicBuffer::DynamicBuffer(int bs){
sem = new Semaphore();
sem->Acquire();
	BlockSize=bs;
	BytesLeft=0;
	BufferSize=0;
	Buffer=0;
	WorkBuffer=0;
	Count=0;
	flags=0;
sem->Release();
}
DynamicBuffer::DynamicBuffer(int bs, int f){
sem = new Semaphore();
sem->Acquire();
	BlockSize=bs;
	BytesLeft=0;
	BufferSize=0;
	Buffer=0;
	WorkBuffer=0;
	Count=0;
	flags=f;
sem->Release();
}
DynamicBuffer::DynamicBuffer(DynamicBuffer *db){
sem = new Semaphore();
sem->Acquire();
	#ifdef _DEBUG
	db->GetBuffer(__FILE__,__LINE__);
	#else
	db->GetBuffer();
	#endif
	BlockSize=db->BlockSize;
	BytesLeft=db->BytesLeft;
	BufferSize=db->BufferSize;
	Buffer=malloc(BufferSize);
	memcpy(Buffer,db->Buffer,BufferSize);
	Count=db->Count;
	WorkBuffer=0;
	flags=db->flags;
sem->Release();
}
DynamicBuffer::~DynamicBuffer(){
sem->Acquire();
	#ifdef _DEBUG
	if(flags&DBUF_SHARED){ if(GetBuffer(__FILE__,__LINE__)) Free(GetBuffer(__FILE__,__LINE__)); }
	else if(GetBuffer(__FILE__,__LINE__)) free(GetBuffer(__FILE__,__LINE__));
	#else
	if(flags&DBUF_SHARED){ if(GetBuffer()) Free(GetBuffer()); }
	else if(GetBuffer()) free(GetBuffer());
	#endif
sem->Release();
delete sem;
}
#ifdef _DEBUG
void DynamicBuffer::Add(void *Source, int Size, char *file, int line){
#else
void DynamicBuffer::Add(void *Source, int Size){
#endif
/*
	while(Size>BytesLeft){
		BytesLeft+=BlockSize;
		BufferSize+=BlockSize;
		Buffer=realloc(Buffer,BufferSize);
	}
	memcpy((void*)(((int)Buffer)+BufferSize-BytesLeft),Source,Size);
	BytesLeft-=Size;
	Count+=Size;
*/
sem->Acquire();
	if(WorkBuffer){
		if(Size>BytesLeft){
			int ofs=BufferSize;
			BufferSize+=BytesInWorkBuffer;
			if(flags&DBUF_SHARED){
				#ifdef _DEBUG
				if(Buffer) Buffer = dbReAllocate(Buffer,BufferSize,file,line);
				else Buffer = dbAllocate(BufferSize,MEM_SHARED,file,line);
				#else
				if(Buffer) Buffer=ReAllocate(Buffer,BufferSize);
				else Buffer = Allocate(BufferSize,MEM_SHARED);
				#endif
			}else Buffer=realloc(Buffer,BufferSize);
			BytesLeft=BlockSize*100;
			memcpy((void*)((int)Buffer+ofs),WorkBuffer,BytesInWorkBuffer);
			BytesInWorkBuffer=0;
		}
//printf("DynamicBuffer::Add(%x,%d) to %x,%d\n",Source,Size,WorkBuffer,BytesInWorkBuffer);
		memcpy((void*)((int)WorkBuffer+BytesInWorkBuffer),Source,Size);
//printf("OK!\n");
		BytesLeft-=Size;
		BytesInWorkBuffer+=Size;
	}else{
		#ifdef _DEBUG
		if(flags&DBUF_SHARED) WorkBuffer = dbAllocate(BlockSize*100,MEM_SHARED,file,line);
		#else
		if(flags&DBUF_SHARED) WorkBuffer = Allocate(BlockSize*100,MEM_SHARED);
		#endif
		else WorkBuffer = malloc(BlockSize*100);
		assert(WorkBuffer);
		BytesLeft  = BlockSize*100;
//printf(".DynamicBuffer::Add(%x,%d) to %x\n",Source,Size,WorkBuffer);
		memcpy(WorkBuffer,Source,Size);
//printf(".OK!\n");
		BytesLeft-=Size;
		BytesInWorkBuffer=Size;
	}
	Count+=Size;
sem->Release();
}

#ifdef _DEBUG
void *DynamicBuffer::GetBuffer(char *file, int line){
#else
void *DynamicBuffer::GetBuffer(void){
#endif
sem->Acquire();
	if(WorkBuffer){
		if(BytesInWorkBuffer){	// ?
			int ofs=BufferSize;
			BufferSize+=BytesInWorkBuffer;
			if(flags&DBUF_SHARED){
				#ifdef _DEBUG
				if(Buffer) Buffer = dbReAllocate(Buffer,BufferSize,file,line);
				else Buffer = dbAllocate(BufferSize,MEM_SHARED,file,line);
				#else
				if(Buffer) Buffer = ReAllocate(Buffer,BufferSize);
				else Buffer = Allocate(BufferSize,MEM_SHARED);
				#endif
			}else Buffer = realloc(Buffer,BufferSize);
			BytesLeft=0;
			memcpy((void*)(int(Buffer)+ofs),WorkBuffer,BytesInWorkBuffer);
			BytesInWorkBuffer=0;
			if(flags&DBUF_SHARED) Free(WorkBuffer);
			else free(WorkBuffer);
			WorkBuffer=0;
		}
	}
sem->Release();
	return(Buffer);
}

int DynamicBuffer::GetCount(void) const{
	return(Count);
}

void DynamicBuffer::Cut(int ofs, int bytes){
sem->Acquire();
	Count-=bytes;
	BytesLeft+=bytes;
	char *dst=(char*)Buffer+ofs;
	char *src=dst+bytes;
	for(int n=0; n<(Count-ofs); n++) *dst++=*src++;
// fixme: Should shrink buffer here
sem->Release();
}

void DynamicBuffer::New(void){
sem->Acquire();
	if(flags&DBUF_SHARED){
		if(Buffer) Free(Buffer);
		if(WorkBuffer) Free(WorkBuffer);
	}else{
		if(Buffer) free(Buffer);
		if(WorkBuffer) free(WorkBuffer);
	}
	BytesLeft=0;
	BufferSize=0;
	Buffer=0;
	Count=0;
	WorkBuffer = 0;
sem->Release();
}
