
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <Drivers/SemaphoreDriver.h>

#define SEMAPHORE_BLOCK_SELF 1

class Semaphore : public SemaphoreDriver{
public:
			Semaphore();
			Semaphore(int);
			~Semaphore();
};
#endif
