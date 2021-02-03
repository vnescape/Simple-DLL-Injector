#include <windows.h>
#include <psapi.h>
#include <iostream>

// Source: https://docs.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes
int PrintProcessID(void) {
	// A pointer to an array that receives the list of process identifiers.
	DWORD aProcesses[512];

	// The number of bytes returned in the pProcessIds array.
	DWORD cbNeeded;

	// Store process identifiers.
	DWORD cProcesses;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		std::cout << "EnumProcesses failed." << std::endl;
		return 1;
	}

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	unsigned int i;
	for (i = 0; i < cProcesses; i++) {
		std::cout << "Process: " << i << "\nID: " << aProcesses[i] << std::endl;
	}
}