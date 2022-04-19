#include "Process.h"
#include <iostream>


int main(int argc, char* argv[]) {
	Process list;
	list.GetProcessIDs();
	for (unsigned int i = 0; i < list.cProcesses; i++) {
		std::cout << "ProcessID: " << list.aProcesses[i] << std::endl;
		std::cout << "ProcessName: ";
		list.PrintProcessName(list.aProcesses[i]);
		std::cout << "\n";
	}
	if (argc < 2) {
		std::cout << "Need more arguments" << std::endl;
		return 1;
	}
	int procID = std::stoi(argv[1]);
	std::string dllPath = argv[2];


	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procID);
	if (hProc == NULL)
	{
		std::cout << "Could not OpenProcess: " << GetLastError();
		return 1;
	}

	LPVOID baseAddress = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (baseAddress == NULL)
	{
		std::cout << "Could not allocate memeory: " << GetLastError() << std::endl;
		return 1;
	}

	if (WriteProcessMemory(hProc, baseAddress, dllPath.c_str(), dllPath.length(), 0) == 0)
	{
		std::cout << "Could not write into process memory: " << GetLastError() << std::endl;
		return 1;
	}


	HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, baseAddress, 0, 0);

	if (hThread)
		CloseHandle(hThread);

	if (hProc) 
		CloseHandle(hProc);

	return 0;
}
