
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

//typedef void (*printf_func)(char *,...);
//typedef void *(*DLLConstruct)(class Process *, class Host *, struct Driver *, printf_func, int adrv, HANDLE con, DynamicArray *);
//__declspec(dllexport) void *Construct(class Process *, class Host *, struct Driver *, printf_func, int adrv, HANDLE con, DynamicArray *);

#include <3dapi/Network/Message.h>

class Process : Message{
public:				Process(char *name);
					~Process();
	//void			FetchMessage(class Message *from, class Message *to);
	char			*GetName(void);
	void			MessageReceived(void);

class Semaphore		*sem;
struct	Driver		*KernelDrivers;
class	Message		*from,*to;
class	Host		*KernelHost;
	int				KernelActiveDriver;
	void			*KernelConsole;

	char			*GetRoot(void) const{ return (char*)Root; };
private:
	char			*Name,*Root;
class DynamicArray	*KernelEntries;
	void			*_dll;
class Semaphore		*quitsem;
};

