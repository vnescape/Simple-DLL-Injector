#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include "Process.h"

int Process::GetUserProcessID(void) {
	std::cout << "Choose a ProcessID to inject to: ";
	std::string ProcID;
	std::getline(std::cin, ProcID);
	std::cout << "You've choosen: " << std::stoi(ProcID) << std::endl;
	return std::stoi(ProcID);
}

std::string Process::GetUserDLLPath(void) {
	std::cout << "Choose a DLL Path to inject to: ";
	std::string DLLPath;
	std::getline(std::cin, DLLPath);
	std::cout << "You've choosen: " << DLLPath << std::endl;
	return DLLPath;
}

int Process::GetProcessIDs(void) {

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		std::cout << "EnumProcesses failed." << std::endl;
		return 1;
	}

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	return 0;
}

int Process::PrintProcessName(int processID) {
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