
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

/*
template <class T> class Ptr {    T* p;public:
    Ptr(T* p_) : p(p_) { p->upcount(); }    ~Ptr(void) { p->downcount(); }
    operator T*(void) { return p; }    T& operator*(void) { return *p; }
    T* operator->(void) { return p; }    Ptr& operator=(T* p_) {
        p->downcount(); p = p_; p->upcount(); return *this;    }};
*/
/*
template <class T> class LeftGarbage : Garbage{
public:		LeftGarbage(T *g){
				garbage = g;
			}
			~LeftGarbage(){}
	void	Jetison(void){ delete garbage; }
private:
	T	*garbage;
};

*/

#include <posix/stdlib.h>

void mem_Leftover(void *);
//void AddGarbageFunc(void(*foo)(void*));
//void RemoveGarbage(DynamicArray *);

template <class T> void Leftover(T *c){
	//new LeftGarbage<T>(c);
	mem_Leftover((void*)c);
//	extern void RemoveGarbage(T *);
//	AddGarbageFunc((void(*)(void*))RemoveGarbage);
//	CollectGarbage(c);
}

void AddGarbageCollector(void(*)(void));

void system_CollectAllGarbage(void);


