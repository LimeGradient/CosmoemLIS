#include "utils/Win32Utils.hpp"

namespace Cosmoem::Utils {
    DWORD Win32Utils::getPIDFromWindowName(const std::wstring &title) {
        HWND hwnd = FindWindowW(NULL, title.c_str());
        if (hwnd == NULL) {
            return 0;
        }

        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        return pid;
    }

    HWND Win32Utils::getWindowFromPID(DWORD pid) {
        WindowData data = { pid, NULL };
        EnumWindows(Win32Utils::EnumWindowsProc, reinterpret_cast<LPARAM>(&data));

        return data.hwnd;
    }

    BOOL CALLBACK Win32Utils::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
        WindowData* data = reinterpret_cast<WindowData*>(lParam);
        DWORD windowPID = 0;
        GetWindowThreadProcessId(hwnd, &windowPID);

        if (
            windowPID == data->targetPID 
            && GetWindow(hwnd, GW_OWNER) == NULL 
            && IsWindowVisible(hwnd)
        ) {
            char className[256];
            GetClassNameA(hwnd, className, sizeof(className));
            if (std::strcmp(className, "ConsoleWindowClass") == 0) {
                return TRUE;
            }

            data->hwnd = hwnd;
            return FALSE;
        }

        return TRUE;
    }
}