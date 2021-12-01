#include "pch.h"

int printTableInfo(DWORD physDiskNumber);
int readHddSectors(void** buffer, int sectorsCount, int physDiskNumber);
BOOL IsAppRunningAsAdminMode();

void SetConsoleColors(int textColor, int backgroundColor);

void showMainMenu();

int main() {
    int errorCheck = _setmode(_fileno(stdout), _O_U16TEXT);

    if (IsAppRunningAsAdminMode() == FALSE) {
        wprintf(L"Restart this application again with administration privelegies!\n");
        return 0;
    }

    showMainMenu();
}

void showMainMenu() {
    const int selTextColor = 0;
    const int deselTextColor = 7;
    const int selBgColor = 1;
    const int deselBgColor = 0;

    int selected = 0;

    const wchar_t** mainMenu = new const wchar_t* [3];
    mainMenu[0] = L"Get disk 1 info\n";
    mainMenu[1] = L"Get disk 2 info\n";
    mainMenu[2] = L"Exit";

    while (true) {
        for (byte i = 0; i < 3; i++)
        {
            if (i == selected) {
                SetConsoleColors(selTextColor, selBgColor);
                wprintf(L"%d) %s", i+1, mainMenu[i]);
                SetConsoleColors(deselTextColor, deselBgColor);
            }
            else
            {
                wprintf(L"%d) %s", i + 1, mainMenu[i]);
            }
        }

        int ch = _getch();
        if (ch == 72)
            --selected;
        if (ch == 80)
            ++selected;
        if (ch == 13) {
            system("cls");
            switch (selected) {
            case 0: printTableInfo(0); getchar(); break;
            case 1: printTableInfo(1); getchar(); break;
            case 2: return;
            }
        }

        if (selected > 2)
            selected = 2;
        else if (selected < 0)
            selected = 0;

        SetConsoleColors(deselTextColor, deselBgColor);
        system("cls");
    }
}

int printTableInfo(DWORD physDiskNumber) {
    const char gptSize = 34; // 1 for protected MBR, 1 for GPT header and 32 for part table

    ptable::gpt* gpt = nullptr;

    int error = readHddSectors((void**)&gpt, gptSize, physDiskNumber);
    
    if (error !=  0) {
        return error;
    }

    if (gpt == nullptr) {
        wprintf(L"An unknown error occurred while reading the GPT!");
        return 0;
    }

    if (ptable::isGptDisk(gpt->pmbr)) {
        ptable::print(gpt);
    }
    else {
        ptable::print(gpt->pmbr, true);
    }

    return 0;
}

int readHddSectors(void** buffer, int sectorsCount, int physDiskNumber) {
    const int bytesInSector = 512;
    std::wstring deviceString = L"\\\\.\\PhysicalDrive" + std::to_wstring(physDiskNumber);
    
    HANDLE hDisk = CreateFile(deviceString.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hDisk == INVALID_HANDLE_VALUE) {
        CloseHandle(hDisk);
        return -1;
    }

    DWORD dwRead = 0;
    
    // we can read only in WHOLE SECTORS
    long bytesToRead = bytesInSector * sectorsCount;

    *buffer = new byte[bytesToRead];

    SetFilePointer(hDisk, 0, NULL, FILE_BEGIN);
    if (!ReadFile(hDisk, *buffer, bytesToRead, &dwRead, NULL) || dwRead != bytesToRead) {
        delete[] *buffer;
        return GetLastError();
    }

    CloseHandle(hDisk);

    return 0;
}

BOOL IsAppRunningAsAdminMode()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    // Allocate and initialize a SID of the administrators group.

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Determine whether the SID of administrators group is enabled in 

    // the primary access token of the process.

    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.

    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    // Throw the error if something failed in the function.

    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return fIsRunAsAdmin;
}

void SetConsoleColors(int textColor, int backgroundColor) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); //получаем наше окно, в котором нужно изменить цвет
    SetConsoleTextAttribute(consoleHandle, (((2 << backgroundColor) | textColor)));
}
