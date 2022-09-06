#include "Process.h"
#include <iostream>
#include <tlhelp32.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

#define PSAPI_VERSION 1
#pragma comment(lib, "psapi.lib")


// https://docs.microsoft.com/en-us/windows/win32/psapi/enumerating-all-modules-for-a-process
int PrintModules(DWORD processID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	// Print the process identifier.

	printf("\nProcess ID: %u\n", processID);

	// Get a handle to the process.

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return 1;

	// Get a list of all the modules in this process.

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.

			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				// Print the module name and handle value.

				_tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
			}
		}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);

	return 0;
}

// Checks if dll is loaded in the target process
int checkForModule(std::string& dllPath, int procId)
{
	// 0 means fals
	return 0;
}


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

	std::cout << "[+] Wrote: " << dllPath.c_str() << " at: " << baseAddress << " via WriteProcessMemory()" << std::endl;

	HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, baseAddress, 0, 0);

	if (hThread == NULL)
	{
		std::cout << "[-] Could not CreateRemoteThread() error: " << GetLastError() << std::endl;
		return 1;
	}

	std::cout << "[+] Created remote thread via CreateRemoteThread()" << std::endl;

	DWORD lpExitCode;
	if (GetExitCodeThread(hThread, &lpExitCode) == NULL)
	{
		std::cout << "[-] Could not GetExitCodeThread() error: " << GetLastError() << std::endl;
		return 1;
	}
	if (lpExitCode == STILL_ACTIVE)
	{
		std::cout << "[+] Thread is running, wait for termination..." << std::endl;
	}

	DWORD waitRes = WaitForSingleObject(hThread, INFINITE);

	if (waitRes == WAIT_FAILED)
	{
		std::cout << "[-] Could not WaitForSingleObject() error: " << GetLastError() << std::endl;
		return 1;
	}

	DWORD lpExitCode1;
	if (GetExitCodeThread(hThread, &lpExitCode1) == NULL)
	{
		std::cout << "[-] Could not GetExitCodeThread() error: " << GetLastError() << std::endl;
		return 1;
	}

	// This return value doesn't seam right.
	// It might be because the Dll loader Thread does return that value
	std::cout << "[+] Thread exited with: " << std::hex << lpExitCode1 << std::endl;

	if (VirtualFreeEx(hProc, baseAddress, 0, MEM_RELEASE) == NULL)
	{
		std::cout << "[-] Could not VirtualFreeEx() error: " << GetLastError() << std::endl;
		return 1;
	}

	if (hThread)
		CloseHandle(hThread);

	if (hProc)
		CloseHandle(hProc);

	return 0;
}

int main(int argc, char** argv) {

	if (argc < 3) {
		std::cout << "Usage: .\\Simple-DLL-Injector.exe <processID / processName> <pathToDll>" << std::endl;
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


	char absolutDllPath[4096] = { 0 };
	if (GetFullPathNameA(argv[2], 4096, absolutDllPath, 0) == NULL)
	{
		std::cout << "[-] Could not GetFullPathNameA() error: " << GetLastError() << std::endl;
		return 1;
	}

	std::string dllPath = absolutDllPath;
	PrintModules(procId);
	return simpleDLLInjection(procId, dllPath);
}
