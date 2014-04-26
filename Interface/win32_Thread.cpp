
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <windows.h>
#include <exception>
#define INTERNAL_DEFINES
#include <posix/stdlib.h>
#include <posix/stdio.h>
#include <posix/unistd.h>
#include <Interface/Thread.h>

// sould we use beginthread? NO!

//#define TEST_THREADS

bool ping_fireman;

static void SpawnThread(Thread *thread){
//printf("SpawnThread\n");
//sleep(5000);
//printf("SpawnThread2\n");
#ifdef TEST_THREADS
thread->started = true;
#endif
//sleep(5000);
//printf("SpawnThread3\n");
	try{
		thread->main();
	}
	catch(class Message *){
		printf("Warning: Unhandled message exception!\n");
	}
	catch(class Socket *){
		printf("Warning: Unhandled socket exception!\n");
	}
	catch(class CMemoryException* e ){
		// Handle the out-of-memory exception here.
		printf("Warning: Unhandled out of memory exception!\n");
	}
	catch(class CFileException* e ){
		// Handle the file exceptions here.
		printf("Warning: Unhandled file exception!\n");
	}
	catch(class CException* e ){
		// Handle all other types of exceptions here.
		printf("Warning: Unhandled CException!\n");
	}
	catch(exception *e){
		printf("exception: %s\n",e->what());
	}
	catch(char *str){
		printf("exception: %s\n",str);
	}
	catch(...){
		printf("Warning: Unhandled exception!\n");
		throw;
	}
	ExitThread(0);
}

Thread::Thread(){
	#ifdef TEST_THREADS
	started = false;
	#endif
	state = RUNING;
//printf("Starting thread!\n");
	thread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)SpawnThread,
		this,
0,//CREATE_SUSPENDED, //		0,
		&threadID);
//printf("Thread = %x, threadID = %x\n",thread,threadID);
	if(!thread){
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL);
		printf("Thread::Thread: Error: %s\n",lpMsgBuf);
		LocalFree(lpMsgBuf);
		extern bool ping_fireman;
		ping_fireman = false;
		return;
	}
//ResumeThread(thread);
	#ifdef TEST_THREADS
	//for(int n=0; n<1; n++){
		sleep(1000);
		if(started) return;
	//}
	printf("!!!!!!!!!!!!!!! Warning !!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!! Thread not started !!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!! Warning !!!!!!!!!!!!!!!!!!!!!!!!\n");
	#endif
}

Thread::Thread(bool){
	#ifdef TEST_THREADS
	started = false;
	#endif
	state = NOT_STARTED;
	thread = 0;
}
Thread::~Thread(){
	Terminate();
}
void Thread::Spawn(void){
//	DWORD dwThreadID;
	state = RUNING;
	thread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)SpawnThread,
		this,
		0,//CREATE_SUSPENDED, //0,
		&threadID);
//printf("Thread = %x, threadID = %x\n",thread,threadID);
	if(!thread){
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL);
		printf("Thread::Spawn: Error: %s\n",lpMsgBuf);
		LocalFree(lpMsgBuf);
		extern bool ping_fireman;
		ping_fireman = false;
		return;
	}
//printf("Thread::Spawn()\n");
//ResumeThread(thread);
	#ifdef TEST_THREADS
	//for(int n=0; n<1; n++){
		sleep(1000);
		if(started) return;
	//}
	printf("!!!!!!!!!!!!!!! Warning !!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!! Thread not started !!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!! Warning !!!!!!!!!!!!!!!!!!!!!!!!\n");
	#endif
}
void Thread::Terminate(void){
//printf("Terminating thread %x\n",thread);
	if(state!=TERMINATED){
		state = TERMINATED;
		if(thread){
			TerminateThread(thread,0);
			CloseHandle(thread);
			thread = 0;
		}
	}
}
void Thread::main(void){
}

