
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdlib.h>
#include <Interface/DynamicArray.h>

static DynamicArray *Garbage = 0;

void AddGarbageCollector(void(*func)(void)){
	if(!Garbage){
		Garbage = new DynamicArray();
		#ifdef _DEBUG
		Garbage->Add(func,__FILE__,__LINE__);
		#else
		Garbage->Add(func);
		#endif
	}
}

void system_CollectAllGarbage(void){
	if(Garbage){
		while(Garbage->GetCount()){
			((void(*)(void))Garbage->GetItem(0))();
			#ifdef _DEBUG
			Garbage->Remove(0,__FILE__,__LINE__);
			#else
			Garbage->Remove(0);
			#endif
		}
		delete Garbage;
	}
}


