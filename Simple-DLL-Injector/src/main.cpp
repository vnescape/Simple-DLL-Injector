#include "Process.h"
#include <iostream>
#include <tlhelp32.h>


int simpleDLLInjection(DWORD& procId, std::string& dllPath) {
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	if (hProc == NULL)
	{
		std::cout << "[-] Could not OpenProcess: " << GetLastError() << std::endl;
		return 1;
	}

	LPVOID baseAddress = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (baseAddress == NULL)
	{
		std::cout << "[-] Could not allocate memeory: " << GetLastError() << std::endl;
		return 1;
	}

	std::cout << "[+] Allocated memory at: " << baseAddress << " via VirtualAllocEx()" << std::endl;

	BOOL write = WriteProcessMemory(hProc, baseAddress, dllPath.c_str(), dllPath.length(), 0);

	if (write == 0)
	{
		std::cout << "[-] Could not write into process memory: " << GetLastError() << " via WriteProcessMemory()" << std::endl;
		return 1;
	}

	std::cout << "[+] Wrote: " << dllPath.c_str() << " at: " << baseAddress << std::endl;

	HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, baseAddress, 0, 0);

	if (hThread == NULL)
	{
		std::cout << "[-] Could not CreateRemoteThread() error: " << GetLastError() << std::endl;
		return 1;
	}

	std::cout << "[+] Started remote thread successfully via CreateRemoteThread()" << std::endl;

	if (hThread)
		CloseHandle(hThread);

	if (hProc)
		CloseHandle(hProc);

	return 0;
}

int main(int argc, char** argv) {

	if (argc < 3) {
		std::cout << "Usage: .\\Simple-DLL-Injector.exe <processID / processName> <absolutPathToDll>" << std::endl;
		return 1;
	}
	
	int argcW;
	LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argcW);

	std::string procName = argv[1];
	DWORD procId = 0;
	
	std::cout << "[ ] Get procId and procName..." << std::endl;

	// detect if process name is used instead of processID
	if (procName.find(".exe") != std::string::npos)
	{
		PROCESSENTRY32 entry;
		// Before calling the Process32First function, set this member to sizeof(PROCESSENTRY32).
		// If you do not initialize dwSize, Process32First fails.
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry) == TRUE)
		{
			while (Process32Next(snapshot, &entry) == TRUE)
			{
				if (wcscmp(entry.szExeFile, argvW[1]) == 0)
				{
					procId = entry.th32ProcessID;
				}
			}
		}
		// very importent to close the handle 
		CloseHandle(snapshot);
		if (procId == 0) {
			std::cout << "[-] Could not find process: " << argv[1] << std::endl;
			return 1;
		}
	}
	else
	{
		procId = std::stoi(argvW[1]);
	}

	std::cout << "[+] Got procId and procName: " << std::endl;
	std::cout << "    |- procId: " << procId << std::endl;
	std::cout << "    |- procName: " << procName << std::endl;

	std::string dllPath = argv[2];

	return simpleDLLInjection(procId, dllPath);
}
