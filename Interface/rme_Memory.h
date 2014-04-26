
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#define MEM_SHARED_WRITE 1
#define MEM_SHARED_READ  2
#define MEM_SHARED (MEM_SHARED_WRITE|MEM_SHARED_READ)

#ifdef _DEBUG
extern void *(*Allocate)(int,int,char*,int);
extern void (*Free)(void*,char*,int);
extern void *(*ReAllocate)(void*,int,char*,int);
inline void *dbAllocate(int a, int b, char *c, int d){ return Allocate(a,b,c,d); }
inline void *dbReAllocate(void *a, int b, char *c, int d){ return ReAllocate(a,b,c,d); }
#define Allocate(x,y) Allocate(x,y,__FILE__,__LINE__)
#define Free(x) Free(x,__FILE__,__LINE__)
#define ReAllocate(x,y) ReAllocate(x,y,__FILE__,__LINE__)
//#define dbAllocate(x,y,a,b) Allocate(x,y,a,b)
#else
extern void *(*Allocate)(int,int);
extern void (*Free)(void*);
extern void *(*ReAllocate)(void*,int);
#endif

void init_Memory(void);
#ifdef _DEBUG
void init_Memory(
	void *(*_Allocate)(int,int, char *file, int line),
	void  (*_Free)(void*, char *file, int line),
	void *(*_ReAllocate)(void*,int, char *file, int line),
	void *_list);
#else
void init_Memory(
	void *(*_Allocate)(int,int),
	void  (*_Free)(void*),
	void *(*_ReAllocate)(void*,int));
#endif
void deinit_Memory(void);

