#include <Windows.h>
#include <string>


BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        std::wstring output = L"Hello from: ";
        WCHAR processName[MAX_PATH] = { 0 };
        if (GetModuleFileNameW(NULL, processName, ARRAYSIZE(processName))) {
            output += processName;
            MessageBox(0, output.c_str(), 0, MB_OK);
        }
        else {
            MessageBox(0, TEXT("Failed to get process "), 0, MB_OK);
        }
        }
        break;

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