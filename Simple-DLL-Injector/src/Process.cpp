#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include "Process.h"

int Process::GetProcessIDs(void) {

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		std::cout << "EnumProcesses failed." << std::endl;
		return 1;
	}

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	return 0;
}

TCHAR Process::PrintProcessName(int processID) {
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL != hProcess) {
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));
		}
		// Print the process name and identifier.
		_tprintf(TEXT("%s\n"), szProcessName);

		// Release the handle to the process.
		CloseHandle(hProcess);
	}
	else {
		std::cout << "\n";
	}
	return 0;
}