#pragma once
#include <Windows.h>
#include <string>

class Process {
	// Source: https://docs.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes
public:
	// A pointer to an array that receives the list of process identifiers.
	DWORD aProcesses[1024];
	// The number of bytes returned in the pProcessIds array.
	DWORD cbNeeded;
	// Store process identifiers.
	DWORD cProcesses;

	int GetUserProcessID(void);
	std::string GetUserDLLPath(void);
	int GetProcessIDs(void);
	int PrintProcessName(int processID);
};