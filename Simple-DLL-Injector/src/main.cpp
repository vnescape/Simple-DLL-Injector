#include "Process.h"
#include <iostream>


int main(void) {
	Process list;
	list.GetProcessIDs();
	for (unsigned int i = 0; i < list.cProcesses; i++) {
		std::cout << "ProcessID: " << list.aProcesses[i] << std::endl;
		std::cout << "ProcessName: ";
		list.PrintProcessName(list.aProcesses[i]);
		std::cout << "\n";
	}
	int procID = list.GetUserProcessID();
	std::string dllPath = list.GetUserDLLPath();


	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procID);

	if (hProc == NULL)
	{
		std::cout << "Could not OpenProcess: " << GetLastError();
	}

	LPVOID baseAddress = VirtualAllocEx(hProc,
										0,
										MAX_PATH,
										MEM_COMMIT | MEM_RESERVE,
										PAGE_READWRITE);
	WriteProcessMemory(hProc, baseAddress, dllPath.c_str(), dllPath.length(), 0);

	std::cin.get();
	return 0;
}
