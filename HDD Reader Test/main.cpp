#include "pch.h"

int readHddSectors(void** buffer, int sectorsCount);

int main()
{
    int errorCheck = _setmode(_fileno(stdout), _O_U16TEXT);

    wprintf(L"Architecture information:\n");
    uint16_t x = 0x0001;
    wprintf(L"Endianness:               %s-endian\n\n", *((uint8_t*)&x) ? L"little" : L"big");

    const char mbrSize = 1;
    const char gptSize = 34; // 1 for protected MBR, 1 for GPT header and 32 for part table

    MBR::mbr* mbr = nullptr;

    errorCheck = readHddSectors((void**)&mbr, mbrSize);

    if (errorCheck != 0) {
        if (errorCheck == 6) {
            std::wcout << L"Start this application again with administration privelegies!\n";
        }
        else {
            std::wcout << L"readHddSectors() -> ReadFile() error " << errorCheck << std::endl;
        }

        return -1;
    }

    if (mbr == nullptr) {
        wprintf(L"An unknown error occurred while reading the MBR!");
        return -1;
    }

    if (!MBR::isGptDisk(*mbr))
    {
        MBR::print(*mbr);
    }
    else {
        GPT::gpt* gpt = nullptr;

        errorCheck = readHddSectors((void**)&gpt, gptSize);

        if (errorCheck != 0) {
            std::wcout << L"readHddSectors() -> ReadFile() error " << errorCheck << std::endl;
            return -1;
        }

        if (gpt == nullptr) {
            wprintf(L"An unknown error occurred while reading the GPT!");
            return -1;
        }

        GPT::print(gpt);
    }

    return 0;
}

int readHddSectors(void** buffer, int sectorsCount) {
    const int bytesInSector = 512;

    HANDLE hDisk = CreateFile(L"\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

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




//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//using namespace std;
//
//void memprt16(void* address, int countOfbytes, int bytesInLine = 32);
//
//int win_error()
//{
//    wcout << "error" << endl;
//    return 0;
//}
//
//int wmain()
//{
//    _setmode(_fileno(stdout), _O_U16TEXT);
//    wcout << "!!! RUN AS ADMINISTRATOR !!!";
//
//    bool fullInfo = false;
//    char answer;
//    wcout << "Full information?\nAnswer (y/n): ";
//    answer = getchar();
//    if (answer == 'y')
//        fullInfo = true;
//
//    HANDLE hDisk = CreateFile(L"\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
//        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//    
//    if (hDisk == INVALID_HANDLE_VALUE) {
//        throw win_error();
//        getchar();
//        getchar();
//        //CloseHandle(hDisk);
//        return 0;
//    }
//
//    byte buffer[512];
//    DWORD dwRead;
//
//    if (!ReadFile(hDisk, buffer, sizeof(buffer), &dwRead, NULL) || dwRead != sizeof(buffer)) {
//        throw win_error();
//        getchar();
//        getchar();
//        //CloseHandle(hDisk);
//        return 0;
//    }
//    CloseHandle(hDisk);
//    
//    MBR::mbr* mbr = (MBR::mbr*)&buffer;
//
//    if (MBR::isGptDisk(*mbr)) {
//        HANDLE hDisk2 = CreateFile(L"\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
//            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//
//
//        dwRead = 0;
//        char mainBuffer[16384]; // ОН ЧИТАЕТ ТОЛЬКО ЦЕЛЫМИ СЕКТОРАМИ!!! Т.Е. КУСКАМИ ПО 512 БАЙТ!!!!
//        
//        SetFilePointer(hDisk2, 0, NULL, FILE_BEGIN);
//        if (!ReadFile(hDisk2, mainBuffer, sizeof(mainBuffer), &dwRead, NULL) || dwRead != sizeof(mainBuffer)) {
//            DWORD error = GetLastError();
//            getchar();
//            return 0;
//        }
//        CloseHandle(hDisk2);
//
//        GPT::gpt gpt;
//        memcpy(&gpt, mainBuffer, sizeof(gpt));
//
//        GPT::print(gpt);
//    }
//    else {
//        CloseHandle(hDisk);
//        MBR::print(*mbr, fullInfo);
//    }
//
//    getchar();
//    getchar();
//}
