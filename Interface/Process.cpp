
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdio.h>
#include <windows.h>
#include <Interface/Debug.h>
#include <Interface/Process.h>
#include <Drivers/Drivers.h>
#include <Interface/rme_Memory.h>
#include <Interface/Semaphore.h>
#include <Interface/globals.h>
#include <Interface/String.h>
//#include "stdafx.h"
//#include <afxdllx.h>

typedef void (*print_func)(char *);
#ifdef _DEBUG
typedef void *(*DLLConstruct)(class Process *, class Host *, struct Driver *, print_func, int adrv, HANDLE con, DynamicArray *, DynamicArray *, struct MemEnt *last, void *(*)(int,int,char*,int), void (*)(void*,char*,int), void *(*)(void*,int,char*,int),double, struct Hook *str_hooks, int *str_masksize, long *str_mask);
#else
typedef void *(*DLLConstruct)(class Process *, class Host *, struct Driver *, print_func, int adrv, HANDLE con, DynamicArray *, DynamicArray *, struct MemEnt *last, void *(*)(int,int), void (*)(void*), void *(*)(void*,int),double, struct Hook *str_hooks, int *str_masksize, long *str_mask);
#endif
//__declspec(dllexport) void *Construct(class Process *, class Host *, struct Driver *, printf_func, int adrv, HANDLE con, DynamicArray *);

Process::Process(char *name) : Message(){

	int len = strlen(name);
	int plen = 0;
	for(int n=0; n<len; n++) if(name[n]=='/') plen = n;
	if(plen){
		Name = new char[len-plen];
		sprintf(Name,"%s",&name[plen+1]);
		Root = new char[plen+1];
		for(int n=0; n<plen; n++) Root[n]=name[n];
		Root[plen]=0;
	}else{
		Name = new char[strlen(name)+1];
		sprintf(Name,"%s",name);
		Root = 0;
	}
	printf("Process: Initializing process %s with root %s\n",Name,Root);

	SetName(Name);
	quitsem = new Semaphore(SEMAPHORE_BLOCK_SELF);
	quitsem->Acquire();
	sem = new Semaphore(SEMAPHORE_BLOCK_SELF);
	sem->Acquire();
	sem->Release();
	sem->Acquire();
printf("proc = %x, sem = %x\n",this,sem);
//	#ifdef _DEBUG
//	char *str = new char[strlen(name)+8];
//	sprintf(str,"db_%s.dll",name);
//	#else
	char *str = new char[strlen(name)+5];
	sprintf(str,"%s.dll",name);
//	#endif
printf("Process: LoadLibrary(\"%s\")\n",str);
	HINSTANCE DLL = LoadLibrary(str);
//	HINSTANCE DLL = AfxLoadLibrary(str);
	_dll = DLL;
	delete str;
	if(DLL){
		DLLConstruct	Construct;
		Construct = (DLLConstruct)GetProcAddress(DLL,"?Constructor@@YAPAXPAVProcess@@PAVHost@@PAUDriver@@P6AXPAD@ZHPAXPAVDynamicArray@@6PAUMemEnt@@P6APAXHH3H@ZP6AX53H@ZP6APAX5H3H@ZNPAUHook@@PAHPAJ@Z");
		if(!Construct) Construct = (DLLConstruct)GetProcAddress(DLL,"?Constructor@@YAPAXPAVProcess@@PAVHost@@PAUDriver@@P6AXPAD@ZHPAXPAVDynamicArray@@6PAUMemEnt@@P6APAXHH@ZP6AX5@ZP6APAX5H@ZNPAUHook@@PAHPAJ@Z");
printf("dll constructor = %x\n",Construct);
		extern Driver *Drivers;
		extern class Host *host;
		extern int ActiveDriver;
		extern HANDLE Win_Console;
		extern DynamicArray *Entries;
		KernelDrivers = Drivers;
		KernelActiveDriver = ActiveDriver;
		KernelConsole = Win_Console;
		KernelEntries = Entries;
printf("Starting proc %x\n",this);
		#ifdef _DEBUG
		//extern DynamicArray *DebugStats;
		//extern struct MemEnt *last;
		#else
		DynamicArray *DebugStats = 0;
		struct MemEnt *last = 0;
		#endif
//		extern struct MemoryHeap *first;
//extern class Heap *first_heap;
//extern class Semaphore *heap_sem;
//extern struct Fragment **Fragments;
		extern print_func stdio_print;
		extern struct Hook *string_hooks;
		extern int *string_masksize;
		extern long *string_mask;
		if(!string_hooks) new String("Dummy");
printf("Calling constructor!\n");
		void *Cache = Construct(this,host,Drivers,stdio_print,ActiveDriver,Win_Console,Entries,0,0,Allocate,Free,ReAllocate,__globals::GetSpeed(),string_hooks,string_masksize,string_mask);
	}else printf("Process: Couldnt open DLL for \"%s\"!\n",name);
}

Process::~Process(){
	// fixme: Exit/Kill process, close DLL etc.
	// Kill
//	quitsem->Acquire();	// fixme: What is this!?
//Sleep(500);				// fixme: Bad!
//	FreeLibrary(_DLL);
	delete sem;
	delete Name;
	if(Root) delete Root;
}

/*
void Process::FetchMessage(Message *_from, Message *_to){
	printf("Process: Sending message..\n");
printf("proc = %x, sem = %x\n",this,sem);
	//from = _from;
	//to = _to;
	//printf("Realeasing message\n");
	sem->Release();
	//printf("Leaving thread\n");
	//Sleep(0);		// Give CPU to other thread
	//printf("Locking message\n");
	sem->Acquire();
}
*/

char *Process::GetName(void){
	return(Name);
}

void Process::MessageReceived(void){
	quitsem->Release();
}

