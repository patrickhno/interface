
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef DEBUG_H
#define DEBUG_H

#include <posix/stdlib.h>
/*
#ifdef _DEBUG
void *fake_malloc(char *, int, int);
void fake_free(void *);
void *fake_realloc(char *, int, void *, int);
#define malloc(x) fake_malloc(__FILE__,__LINE__,x)
#define realloc(x,y) fake_realloc(__FILE__,__LINE__,x,y)
#define free(x) fake_free(x)

//#include <stdlib.h>

#define __MEMDEBUG__
#else
*/
//#define DONT_OVERIDE_JUNK_COMPILERS
//#include <stdlib.h>
//#endif
//#include <stdio.h>
//#include <stdarg.h>
//#include <3dapi/Network/Host.h>
//#include "Kernel.h"

//#undef printf
//#undef puts

//	typedef void (*printf_func)(char *);
//	extern printf_func *my_printf;
/*
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

static void puts(char *fmt){
	char str[1024];
	sprintf(str,"%s\n",fmt);
//	extern Native *__global__Native__;
//	__global__Native__->GetHost()->MessageReceived(0,__global__Native__->GetHost()->GetHook("output"),str);
//	void Console_Printf(char *);
//	Console_Printf(str);
	extern void (*stdio_print)(char *);
	stdio_print(str);
}
*/
/*
#ifdef _DEBUG

//static void *operator new(unsigned int size){
//	return malloc(size);
//}

#undef new
static void *operator new(unsigned int size, char *FileName, int nLine){
	return fake_malloc(FileName,nLine,size);
}
#define new new(__FILE__,__LINE__)

static void operator delete(void *buf){
	free(buf);
}
#endif
*/
#endif

