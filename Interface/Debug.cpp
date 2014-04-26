
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifdef _DEBUG

#include <posix/stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "Kernel.h"
#include "rme_Memory.h"
#include "DynamicArray.h"
#include <assert.h>
#include "Semaphore.h"
#include <Interface/Garbage.h>

struct MemEnt{
	MemEnt	*Next;
	MemEnt	*Prev;
	int		Size;
	char	*File;
//	int		Line;
	void	*Buffer;
	int		line;
};

// Do not share this!
static MemEnt *last;
static int MemoryInUse;
static int MaxUsage;
static Semaphore *sem;

//int filecount;

struct FileStats{
	char	*FileName; //FileName[1024];
	int		InUse;
	int		MaxUse;
struct Line *Lines;
};

struct Line{
	Line	*Next;
	int		Bytes;
	int		line;
};

//FileStats *Stats;
DynamicArray *DebugStats = 0;

void Initialize_Debug(void){
	last = new MemEnt;
	Leftover(last);	// fixme: Why?
	last->Next = 0;
	last->Prev = 0;
	last->Size = 0;
//	last->File = 0;
//	last->Line = 0;
	last->Buffer = 0;
	MemoryInUse = MaxUsage = 0;
	//filecount=0;
	//Stats=0;
	DebugStats = new DynamicArray(DARRAY_SHARED_MEMORY);
	Leftover(DebugStats);
	sem = new Semaphore();
	Leftover(sem);
}

static void printf(char *fmt,...){
	char str[1024];
	va_list args;
	va_start(args,fmt);
	vsprintf(str,fmt,args);
//	extern Native *__global__Native__;
//	__global__Native__->GetHost()->MessageReceived(0,__global__Native__->GetHost()->GetHook("output"),str);
//	void Console_Printf(char *);
//	Console_Printf(str);
	extern void (*stdio_print)(char *);
	stdio_print(str);
}

void Deinitialize_Debug(void){
	if(last){
		int cnt=1;
//		while(last->Prev){
//			cnt++;
//			last=last->Prev;
//		}
last = last->Next;
		int size=1024;
		//char *str = (char*)malloc(size);
		//sprintf(str,"Max memory usage: %d\nWarning: %d memory leaks found!\n",MaxUsage,cnt);
		//sprintf(&str[strlen(str)],"Stats:\n");

		printf("Max memory usage: %d\nWarning: %d memory leaks found!\n",MaxUsage,cnt);
		printf("Stats:\n");

	int filecount = DebugStats->GetCount();
		for(int n=0; n<filecount; n++){
			//sprintf(&str[strlen(str)],"\"%s\":\r\t\t\t%d bytes\n",Stats[n].FileName,Stats[n].MaxUse);
			//printf("\"%s\":\r\t\t\t%d bytes\n",Stats[n].FileName,Stats[n].MaxUse);
			FileStats *Stats = (FileStats*)DebugStats->GetItem(n);
			printf("\"%s\":\t%d bytes\n",Stats->FileName,Stats->MaxUse);
		}
		printf("\nMemory leaks:\n");
//		while(last){
			//if((strlen(str)-size)>1024){
			//	str=(char*)realloc(str,strlen(str)+1024);
			//	size=strlen(str)+1024;
			//}
			//if(last->Size>1024)
			//sprintf(&str[strlen(str)],"%s(%d) %d bytes at 0x%x\n",last->File,last->Line,last->Size,(int)last+sizeof(MemEnt));
//			/*if(strcmp(last->File,"../Interface\\Debug.h"))*/ printf("%s(%d) %d bytes at 0x%x\n",last->File,last->Line,last->Size,last->Buffer);
//			last=last->Next;
//		}
		//return str;

		cnt = DebugStats->GetCount();
		for(n=0; n<cnt; n++){
			FileStats *Stats = (FileStats*)DebugStats->GetItem(n);
			int bytes = 0;
			Line *l = Stats->Lines;
			while(l){
				bytes += l->Bytes;
				l=l->Next;
			}
			if(bytes){
				printf("In file \"%s\":\n",Stats->FileName);
				Line *l = Stats->Lines;
				while(l){
					if(l->Bytes) printf("%d bytes at line %d\n",l->Bytes,l->line);
					l=l->Next;
				}
			}
		}

	}
	delete DebugStats;
	delete sem;
//while(true);
}

static char *AddToFile(char *file, int line, int size){
MemEnt *l = last;
while(l->Next) l=l->Next;
//	if(Stats){
		// Seek
	int filecount = DebugStats->GetCount();
		for(int n=0; n<filecount; n++){
			FileStats *Stats = (FileStats*)DebugStats->GetItem(n);
//			if(!strcmp(file,Stats->FileName)){
			if(file==Stats->FileName){
				Stats->InUse+=size;
				if(Stats->InUse>Stats->MaxUse) Stats->MaxUse = Stats->InUse;
				Line *l = Stats->Lines;
				Line *last = l;
				while(l){
					if(l->line == line){
						l->Bytes += size;
l = last;
while(l->Next) l=l->Next;
						return Stats->FileName;
					}
					last = l;
					l=l->Next;
				}
				last->Next = new Line;
				last->Next->Next  = 0;
				last->Next->Bytes = size;
				//last->Next->Name  = new char[strlen(file)+1];
				last->Next->line  = line;
				//sprintf(last->Next->Name,"%s",Name);
l = last;
while(l->Next) l=l->Next;
				return Stats->FileName;
			}
		}
		//Stats=(FileStats*)ReAllocate(Stats,sizeof(FileStats)*(filecount+1));
		FileStats *Stats = (FileStats*)Allocate(sizeof(FileStats),MEM_SHARED);
/*
		sprintf(Stats->FileName,"%s",file);
*/
		Stats->FileName = file;

		Stats->InUse=size;
		Stats->MaxUse=size;
		Stats->Lines = new Line;
		Stats->Lines->Next = 0;
		Stats->Lines->Bytes = size;
		//Stats->Lines->Name  = new char[strlen(file)+1];
		Stats->Lines->line  = line;
		//sprintf(Stats->Lines->Name,"%s",Name);
		#ifdef _DEBUG
		DebugStats->Add(Stats,__FILE__,__LINE__);
		#else
		DebugStats->Add(Stats);
		#endif
l = last;
while(l->Next) l=l->Next;
		return Stats->FileName;
//	}else{
//		Stats = (FileStats*)Allocate(sizeof(FileStats),MEM_SHARED);
//		sprintf(Stats[0].FileName,"%s",file);
//		Stats[0].InUse=size;
//		Stats[0].MaxUse=size;
//	}
//	filecount++;
}

void *fake_malloc(char *file, int line, int size){
MemEnt *l = last;
while(l->Next) l=l->Next;
sem->Acquire();
l = last;
while(l->Next) l=l->Next;

	MemEnt *ret=(MemEnt*)Allocate(sizeof(MemEnt),MEM_SHARED);

l = last;
while(l->Next) l=l->Next;

	ret->Buffer = malloc(size);

	l = last;
	while(l->Next) l=l->Next;

	ret->Next=0;
	ret->Prev=l; //last;
	ret->Size=size;

	l->Next = ret;

	MemoryInUse+=size;
	if(MemoryInUse>MaxUsage) MaxUsage=MemoryInUse;

ret->File =	AddToFile(file,line,size);
sem->Release();

	ret->line = line;

	//return((void*)((int)ret+sizeof(MemEnt)));
l = last;
while(l->Next) l=l->Next;

	return ret->Buffer;
}

void fake_free(void *buf){
MemEnt *l = last;
while(l->Next) l=l->Next;
	static int cnt = 0;
sem->Acquire();
assert(cnt==0);
cnt++;
	//MemEnt *me = (MemEnt*)((int)buf-sizeof(MemEnt));
	MemEnt *me = last->Next;
	assert(me!=0);
	while(me->Buffer!=buf){
		me=me->Next;
		assert(me!=0);
	}

	if(me->Next) me->Next->Prev=me->Prev;
	if(me->Prev) me->Prev->Next=me->Next;
	if(last==me) last=me->Prev;

	MemoryInUse-=me->Size;
	if(MemoryInUse>MaxUsage) MaxUsage=MemoryInUse;

	AddToFile(me->File,me->line,-me->Size);

	free(me->Buffer);
//	Free(me->File);
	Free(me);
cnt--;
l = last;
while(l->Next) l=l->Next;
sem->Release();
l = last;
while(l->Next) l=l->Next;
}

void *fake_realloc(char *file, int line, void *buf, int size){
MemEnt *l = last;
while(l->Next) l=l->Next;
	if(buf==0) return(fake_malloc(file,line,size));
l = last;
while(l->Next) l=l->Next;
sem->Acquire();
/*
	MemEnt *old = (MemEnt*)((int)buf-sizeof(MemEnt));
	MemEnt *me = (MemEnt*)realloc((void*)((int)buf-sizeof(MemEnt)),sizeof(MemEnt)+size);

	if(me->Next) me->Next->Prev=me;
	if(me->Prev) me->Prev->Next=me;
	MemoryInUse-=me->Size;
	MemoryInUse+=size;
	if(MemoryInUse>MaxUsage) MaxUsage=MemoryInUse;
	AddToFile(file,size-me->Size);
	me->Size=size;
	if(old==last) last=me;

	me->File = file;
	me->Line = line;

	return((void*)((int)me+sizeof(MemEnt)));
*/
	MemEnt *me = last->Next;
	while(me->Buffer!=buf) me=me->Next;
l = last;
while(l->Next) l=l->Next;
sem->Release();
l = last;
while(l->Next) l=l->Next;
	return me->Buffer = realloc(me->Buffer,size);
}

#endif
