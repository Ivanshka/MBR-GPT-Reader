#include "pch.h"
#include "ConsoleMenu.h"

int printTableInfo(DWORD physDiskNumber);
int readHddSectors(void** buffer, int sectorsCount, int physDiskNumber);
BOOL IsAppRunningAsAdminMode();

void SetConsoleColors(int textColor, int backgroundColor);

void printDisk1Info();
void printDisk2Info();

int getHDDAmount();

ConsoleMenu* getMainMenu() {
    ConsoleMenu* menu = new ConsoleMenu(L"Exit");

    ConsoleMenuItem item1(L"Get disk 1 information\n", printDisk1Info);
    ConsoleMenuItem item2(L"Get disk 2 information\n", printDisk2Info);

    menu->addMenuItem(&item1);
    menu->addMenuItem(&item2);

    return menu;
}

int main() {
    int errorCheck = _setmode(_fileno(stdout), _O_U16TEXT);

    int hddAmount = getHDDAmount();
    wprintf(L"Physical disks amount is %d\n", hddAmount);

    if (IsAppRunningAsAdminMode() == FALSE) {
        wprintf(L"Restart this application again with administration privelegies!\n");
        return 0;
    }

    ConsoleMenu* menu = getMainMenu();
    menu->show();
    delete menu;

    return 0;
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


void printDisk1Info() {
    wprintf(L"\n");
    const int number = 0;
    printTableInfo(number);
    _getch();
}

void printDisk2Info() {
    wprintf(L"\n");
    const int number = 1;
    printTableInfo(number);
    _getch();
}

int getHDDAmount()
{
    // BAD WAY
    int diskNumber = 0;
    bool valid = false;
    do {
        std::wstring deviceString = L"\\\\.\\PhysicalDrive" + std::to_wstring(diskNumber);

        HANDLE hDisk = CreateFile(deviceString.c_str(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hDisk != INVALID_HANDLE_VALUE) {
            valid = true;
            ++diskNumber;
        }
        else
            valid = false;

    } while (valid);

    return diskNumber;
}

