#include "Process.h"
#include <iostream>
#include <tlhelp32.h>


int simpleDLLInjection(int procID, std::string dllPath) {
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

int main(int argc, char** argv) {

	if (argc < 3) {
		std::cout << "Usage: .\\Simple - DLL - Injector.exe <processID / processName> <pathToDLL>" << std::endl;
		return 1;
	}
	
	std::string procName = argv[1];
	int procID = 0;
	// detect if process name is used instead of processID
	if (procName.find(".exe") != std::string::npos)
	{
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry) == TRUE)
		{
			while (Process32Next(snapshot, &entry) == TRUE)
			{
				if (wcscmp(entry.szExeFile, (wchar_t*)procName.c_str()) == 0)
				{
					procID = entry.th32ProcessID;
				}
			}
		}

		CloseHandle(snapshot);
	}
	else
	{
		procID = std::stoi(argv[1]);
	}


	std::string dllPath = argv[2];
	

	return simpleDLLInjection(procID, dllPath);
}
