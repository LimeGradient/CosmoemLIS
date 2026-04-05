#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>

DWORD getPID(const wchar_t* name) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W e{ sizeof(e) };
    while (Process32NextW(snap, &e))
        if (wcscmp(e.szExeFile, name) == 0) { CloseHandle(snap); return e.th32ProcessID; }
    CloseHandle(snap);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc <= 2) {
        std::cout << "No CMD Args given. Launch with EXE path and DLL path." << std::endl;
        return 0;
    }

    const wchar_t* TARGET   = L"Life is Strange - Before the Storm.exe";
    const char* EXE_PATH = argv[1];
    const char* DLL_PATH = argv[2];

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    CreateProcess(
        EXE_PATH,
        0,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    DWORD pid = getPID(TARGET);
    if (!pid) { 
        std::cout << "Process not found\n"; return 1; 
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    void* remoteMem = VirtualAllocEx(
        hProc, 
        nullptr, 
        strlen(DLL_PATH) + 1,
        MEM_COMMIT, PAGE_READWRITE
    );
    WriteProcessMemory(hProc, remoteMem, DLL_PATH, strlen(DLL_PATH) + 1, nullptr);

    HANDLE hThread = CreateRemoteThread(
        hProc, 
        nullptr, 
        0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"),
        remoteMem, 
        0,
         nullptr
    );

    WaitForSingleObject(hThread, 1000);
    VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProc);

    std::cout << "Injected!" << std::endl;
}