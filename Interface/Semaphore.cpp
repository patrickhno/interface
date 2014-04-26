
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include "Semaphore.h"

Semaphore::Semaphore() : SemaphoreDriver(0){}
Semaphore::Semaphore(int f) : SemaphoreDriver(f){}
Semaphore::~Semaphore(){}

