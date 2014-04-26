
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <windows.h>
#define INTERNAL_DEFINES
#include <posix/stdlib.h>
#include <posix/stdio.h>

bool win32_QueryPerformanceFrequency(__int64 *t){
	return QueryPerformanceFrequency((LARGE_INTEGER*)t);
}
bool win32_QueryPerformanceCounter(__int64 *t){
	return QueryPerformanceCounter((LARGE_INTEGER*)t);
}

void win32_Reboot(void){
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) printf("OpenProcessToken\n");

	// Get the LUID for the shutdown privilege. 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the shutdown privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
	// Cannot test the return value of AdjustTokenPrivileges. 
	if(GetLastError() != ERROR_SUCCESS) printf("AdjustTokenPrivileges\n"); 

	// Shut down the system and force all applications to close. 
	if(!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0)) printf("ExitWindowsEx\n"); 
} 

