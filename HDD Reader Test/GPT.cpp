#include "pch.h"
#include "gpt.h"
#include <string>

LPWSTR asciiToUnicode(char* asciiString) {
    size_t len = strlen(asciiString);
    PWCHAR uniString = new WCHAR[len + 1];
    int result = MultiByteToWideChar(CP_OEMCP, 0, asciiString, -1, uniString, len + 1);
    return uniString;
}

void memLEHexPrt(void* address, int countOfbytes, int bytesInLine = 32);
void printGuid(byte guid[16]);

void GPT::print(GPT::gpt* gpt)
{
    wprintf(L"GPT header information:\n");

    wprintf(L"Signature:                %s\n", asciiToUnicode((char*)gpt->header.signature1));
    wprintf(L"GPT header version:       ");

    if (gpt->header.headerVersion2[0] != 0)
        wprintf(L"%X.%X.%X.%X\n", gpt->header.headerVersion2[0], gpt->header.headerVersion2[1],
            gpt->header.headerVersion2[2], gpt->header.headerVersion2[3]);

    else if (gpt->header.headerVersion2[1] != 0)
        wprintf(L"%X.%X.%X\n", gpt->header.headerVersion2[1], gpt->header.headerVersion2[2],
            gpt->header.headerVersion2[3]);

    else if (gpt->header.headerVersion2[2] != 0)
        wprintf(L"%X.%X\n", gpt->header.headerVersion2[2], gpt->header.headerVersion2[3]);

    else if (gpt->header.headerVersion2[3] != 0)
        wprintf(L"%X\n", gpt->header.headerVersion2[3]);

    wprintf(L"Header size:              %d\n", gpt->header.headerSize3);
    wprintf(L"Header CRC32 hash:        %X\n", gpt->header.headerCrc32Hash4);
    wprintf(L"GPT 1 header address:     %X\n", *(long*)gpt->header.primaryGptHeaderAddress6);
    wprintf(L"GPT 2 header address:     %X\n", *(long*)gpt->header.secondaryGptHeaderAddress7);
    wprintf(L"First part address:       %X\n", *(long*)gpt->header.firstPartitionAddress8);
    wprintf(L"Last part sector:         %X\n", *(long*)gpt->header.lastPartitionSectorOfDisk9);
    wprintf(L"Disk GUID:                "); printGuid(gpt->header.diskGuid10); wprintf(L"\n");
    wprintf(L"Part table address:       %X\n", *(long*)gpt->header.partitionTableAddress11);
    wprintf(L"Max partitions amount:    %X\n", gpt->header.maxPartitionAmount12);
    wprintf(L"Partition record size:    %X\n", gpt->header.partitionRecordSize13);
    wprintf(L"Part table CRC32 hash:    %X\n", gpt->header.partTableCrc32Hash14);

    wprintf(L"\nPartitions information:\n");
    for (int i = 0; i < 128; ++i) { // max amount of partitions = 128
        // skip empty records
        if (*(long long*)gpt->partitionTable[i].typeGuid == 0 &&
            *((long long*)gpt->partitionTable[i].typeGuid + 1) == 0)
            continue;

        wprintf(L"Partition record #%d\n", i + 1);
		GPT::print(&gpt->partitionTable[i]);
        if (i != 127)
            wprintf(L"\n");
    }
}

// https://ru.wikipedia.org/wiki/Таблица_разделов_GUID#Записи_данных_о_разделах_(LBA_2-33)
// https://habr.com/ru/post/347002/
// we can add string "Partition type", cause partition type can be identified using guid of type and guid's table in the wiki
void GPT::print(GPT::partition* part)
{
    int partNameLength = 36;
    
    wprintf(L"Name:                     ");
    
    for (int i = 0; i < partNameLength; i++) {
        wprintf(L"%c", part->partitionName[i]);
    }
    wprintf(L"\n");

    wprintf(L"Type GUID:                "); printGuid(part->typeGuid); wprintf(L"\n");
    wprintf(L"Partition GUID:           "); printGuid(part->partitionGuid); wprintf(L"\n");
}

void printGuid(byte guid[16]) {
    
    int hexPart;

    for (int i = 3; i > -1; i--) {
        hexPart = (guid[i] & 240) >> 4; // 1111 0000 -> 0000 1111
        wprintf(L"%X", hexPart);
        hexPart = guid[i] & 15;  // 0000 1111
        wprintf(L"%X", hexPart);
    }

    wprintf(L"-");
    
    for (int i = 4; i < 8; i += 2) {
        hexPart = (guid[i + 1] & 240) >> 4;
        wprintf(L"%X", hexPart);
        hexPart = guid[i + 1] & 15;
        wprintf(L"%X", hexPart);

        hexPart = (guid[i] & 240) >> 4;
        wprintf(L"%X", hexPart);
        hexPart = guid[i] & 15;
        wprintf(L"%X-", hexPart);
    }

    for (int i = 8; i < 16; i++) {
        hexPart = (guid[i] & 240) >> 4;
        wprintf(L"%X", hexPart);
        hexPart = guid[i] & 15;
        wprintf(L"%X", hexPart);
        if (i == 9)
            wprintf(L"-");
    }
}

void memLEHexPrt(void* address, int countOfbytes, int bytesInLine) {
    char* memory = (char*)address;
    int hexPart1;
    int hexPart2;
    int center = bytesInLine / 2;

    for (int i = countOfbytes - 1; i > -1; i--) {
        hexPart1 = (memory[i] & 240) >> 4; // 1111 0000 -> 0000 1111
        hexPart2 = memory[i] & 15;  // 0000 1111

        if ((i + 1) % bytesInLine == 0)
            wprintf(L"%X%X\n", hexPart1, hexPart2);
        else if ((i + 1) % center == 0)
            wprintf(L"%X%X  ", hexPart1, hexPart2);
        else
            wprintf(L"%X%X ", hexPart1, hexPart2);
    }
}
