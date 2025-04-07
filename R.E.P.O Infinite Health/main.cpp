#include <Windows.h>
#include <iostream>
#include <vector>
#include "proc.h"
#include "mem.h"

int main()
{
    HANDLE hProcess = 0;
    uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
    bool bHealth = false;

    const int newValue = 69420;

    DWORD procId = GetProcId(L"REPO.exe");

    if (procId)
    {
        hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE, NULL, procId);
        if (!hProcess) {
            std::cerr << "Failed to open process!" << std::endl;
            return 0;
        }
        moduleBase = GetModuleBaseAddress(procId, L"UnityPlayer.dll");
        if (!moduleBase) {
            std::cerr << "Failed to get module base address!" << std::endl;
            CloseHandle(hProcess);
            return 0;
        }
        localPlayerPtr = moduleBase + 0x01d1f180;
        healthAddr = FindDMAADDY(hProcess, localPlayerPtr, { 0x0, 0x3A0, 0x0, 0x0, 0xA0, 0x28, 0xAC });
        if (!healthAddr) {
            std::cerr << "Failed to resolve health address!" << std::endl;
            CloseHandle(hProcess);
            return 0;
        }
    }
    else
    {
        std::cout << "Process not found. Press enter to exit \n";
        getchar();
        return 0;
    }

    DWORD dwExit = 0;

    while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
    {
        if (GetAsyncKeyState(0x47) & 1)  // 'G'
        {
            bHealth = !bHealth;
        }
        if (GetAsyncKeyState(0x4B) & 1)  // 'K'
        {
            break;  // Exit
        }
        if (bHealth)
        {
            mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
        }
        Sleep(10);
    }

    std::cout << "Process not found or exited. Press Enter to exit.\n";
    getchar();
    CloseHandle(hProcess);
    return 0;
}
