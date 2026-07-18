#pragma once

#include <Windows.h>
#include <string>

namespace Cosmoem::Utils {
    class Win32Utils {
    public:
        static DWORD getPIDFromWindowName(const std::wstring& title);
        static HWND getWindowFromPID(DWORD pid);
        
    private:
        struct WindowData {
            DWORD targetPID;
            HWND hwnd;
        };

        static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    };
}