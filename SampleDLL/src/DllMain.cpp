#include <Windows.h>
#include <iostream>
#include <string>

int start(HINSTANCE hinstDLL) {
    std::wstring output = L"Hello from: ";
    WCHAR processName[MAX_PATH] = { 0 };

    if (GetModuleFileNameW(NULL, processName, ARRAYSIZE(processName))) {
        output += processName;
        MessageBox(0, output.c_str(), TEXT("MessageBox"), MB_OK);
    }
    else {
        MessageBox(0, TEXT("Failed to get process "), TEXT("MessageBox"), MB_OK);
    }

    //leave
    FreeLibraryAndExitThread(hinstDLL, 0);
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.-
        // Return FALSE to fail DLL load.
    {
        HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)start, hinstDLL, 0, nullptr);

        if (hThread == NULL) {
            std::cout << "Could not create thread: " << GetLastError() << std::endl;
            return 1;
        }


        CloseHandle(hThread);
        break;
    }

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}