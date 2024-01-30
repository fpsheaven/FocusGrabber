//
// Licence: public domain - unrestricted usage
//
#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <fcntl.h>
#include <io.h>

int main()
{
    WCHAR program[MAX_PATH];
    DWORD activeProcId = 0;
    DWORD lastActiveProcId = 0;

    _setmode(_fileno(stdout), _O_U16TEXT);

    while (true) {

        Sleep(100);
        activeProcId = 0;

        HWND hWnd = GetForegroundWindow();
        if (hWnd != NULL) {
            GetWindowThreadProcessId(hWnd, &activeProcId);
            if (activeProcId == 0) {
                wprintf(L"GetWindowThreadProcessId had error %u\n", GetLastError());
                continue;
            }

            HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, activeProcId);
            if (hProc == NULL) {
                wprintf(L"OpenProcess had error %u\n", GetLastError());
                continue;
            }

            DWORD rc = GetModuleFileNameExW((HMODULE)hProc, NULL, program, MAX_PATH);
            if (rc == 0) {
                wprintf(L"GetModuleFileNameExW had error %u\n", GetLastError());
                CloseHandle(hProc);
                continue;
            }

            CloseHandle(hProc);
        }


        if (activeProcId != lastActiveProcId) {
            WCHAR date[256];
            time_t now = time(0);
            struct tm t;
            localtime_s(&t, &now);
            wcsftime(date, sizeof(date)/sizeof(WCHAR), L"%a %d %B %T", &t);
            if (activeProcId == 0)
                wprintf(L"No foreground application | %s\n", date);
            else
                wprintf(L"%u:%s | %s\n", activeProcId, program, date);

            lastActiveProcId = activeProcId;
        }

    }

}
