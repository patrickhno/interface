
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include "Debug.h"

#include "KernelClient.h"
#include "Kernel.h"

KernelClient::KernelClient(){
}
KernelClient::~KernelClient(){
}

double KernelClient::Time(void){
	extern Kernel *__global__Kernel__;
	return(__global__Kernel__->Time());
}

