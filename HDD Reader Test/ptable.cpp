#include "pch.h"

wchar_t* typeCodeToWstring(byte partTypeCode);
LPWSTR asciiToUnicode(char* asciiString);
LPWSTR guidToString(byte guid[16]);

bool ptable::isGptDisk(ptable::mbr mbr)
{
    return mbr.partitions[0].type == 0xEE;
}

void ptable::print(ptable::mbrPartitionRecord part, bool fullInfo)
{
    wprintf(L"Active:                                  %s\n", part.isActive == 0x80 ? L"Yes" : L"No");
    if (fullInfo) {
        wprintf(L"Partition beginning, head:               %X\n", part.beginHead);
        wprintf(L"Partition beginning, sector-cylinder:    %X %X\n", part.beginSectorCylinder[0], part.beginSectorCylinder[0]);
    }
    wprintf(L"Type, code:                              %X\n", part.type);
    wprintf(L"Type, description:                       %s\n", typeCodeToWstring(part.type));
    if (fullInfo) {
        wprintf(L"Partition end, head:                     %X\n", part.endHead);
        wprintf(L"Partition end, sector-cylinder:          %X %X\n", part.endSectorCylinder[0], part.endSectorCylinder[0]);
    }
    wprintf(L"First sector offset:                     %d\n", *(int*)part.firstSectorOffset);
    wprintf(L"Partition sector amount:                 %d\n", *(int*)part.partitionSectorAmount);
}

void ptable::print(ptable::mbr mbr, bool fullInfo)
{
    if (mbr.partitions[0].type == 0xEE)
    {
        wprintf(L"Disk type is GPT so disk has PMBR (protected MBR) with 1 partition!\n");
        print(mbr.partitions[0]);
        return;
    }

    for (int i = 0; i < 4; ++i) {
        // skip empty records
        if (mbr.partitions[i].type != 0) {
            wprintf(L"Partition %d\n", i);
            print(mbr.partitions[i], fullInfo);
            wprintf(L"\n");
        }
    }
}

void ptable::print(ptable::gpt* gpt)
{
    wprintf(L"GPT header information:\n");

    wprintf(L"Signature:                %s\n", asciiToUnicode((char*)gpt->header.signature));
    wprintf(L"GPT header version:       ");

    if (gpt->header.headerVersion[0] != 0)
        wprintf(L"%X.%X.%X.%X\n", gpt->header.headerVersion[0], gpt->header.headerVersion[1],
            gpt->header.headerVersion[2], gpt->header.headerVersion[3]);

    else if (gpt->header.headerVersion[1] != 0)
        wprintf(L"%X.%X.%X\n", gpt->header.headerVersion[1], gpt->header.headerVersion[2],
            gpt->header.headerVersion[3]);

    else if (gpt->header.headerVersion[2] != 0)
        wprintf(L"%X.%X\n", gpt->header.headerVersion[2], gpt->header.headerVersion[3]);

    else if (gpt->header.headerVersion[3] != 0)
        wprintf(L"%X\n", gpt->header.headerVersion[3]);

    wprintf(L"Header size:              %d\n", gpt->header.headerSize);
    wprintf(L"Header CRC32 hash:        %X\n", gpt->header.headerCrc32Hash);
    wprintf(L"GPT 1 header address:     %X\n", *(long*)gpt->header.primaryGptHeaderAddress);
    wprintf(L"GPT 2 header address:     %X\n", *(long*)gpt->header.secondaryGptHeaderAddress);
    wprintf(L"First part address:       %X\n", *(long*)gpt->header.firstPartitionAddress);
    wprintf(L"Last part sector:         %X\n", *(long*)gpt->header.lastPartitionSectorOfDisk);
    wprintf(L"Disk GUID:                %s\n", guidToString(gpt->header.diskGuid));
    wprintf(L"Part table address:       %X\n", *(long*)gpt->header.partitionTableAddress);
    wprintf(L"Max partitions amount:    %X\n", gpt->header.maxPartitionAmount);
    wprintf(L"Partition record size:    %X\n", gpt->header.partitionRecordSize);
    wprintf(L"Part table CRC32 hash:    %X\n", gpt->header.partTableCrc32Hash);

    wprintf(L"\nPartitions information:\n");
    for (int i = 0; i < 128; ++i) { // max amount of partitions = 128
        // skip empty records
        if (*(long long*)gpt->partitionTable[i].typeGuid == 0 &&
            *((long long*)gpt->partitionTable[i].typeGuid + 1) == 0)
            continue;

        wprintf(L"Partition record #%d\n", i + 1);
		ptable::print(&gpt->partitionTable[i]);
        if (i != 127)
            wprintf(L"\n");
    }
}

// https://ru.wikipedia.org/wiki/Таблица_разделов_GUID#Записи_данных_о_разделах_(LBA_2-33)
// https://habr.com/ru/post/347002/
// we can add string "Partition type", cause partition type can be identified using guid of type and guid's table in the wiki
void ptable::print(ptable::gptPartitionRecord* part)
{
    wprintf(L"Name:                     %s\nType GUID:                %s\nPartition GUID:           %s\n",
        part->partitionName, guidToString(part->typeGuid), guidToString(part->partitionGuid));
}

wchar_t* typeCodeToWstring(byte partTypeCode) {
    std::wstring type;
    switch (partTypeCode) {
    case 0x0: type = L"Free space"; break;
    case 0x1: type = L"FAT-12"; break;
    case 0x4: type = L"FAT-16B / FAT-16 / FAT-12"; break;
    case 0x5: type = L"Extended partition"; break;
    case 0x7: type = L"IFS, HPFS, NTFS, exFAT or other"; break;
    case 0xB: type = L"FAT-32"; break;
    case 0xC: type = L"FAT-32X (FAT-32 with LBA)"; break;
    case 0xF: type = L"Extended partition with LBA"; break;
    case 0x27: type = L"Hidden NTFS (system recovery partition)"; break;
    case 0x41: type = L"Old Linux / Minix, PPC PReP Boot"; break;
    case 0x42: type = L"Old Linux swap, SFS, Windows Dynamic Disk"; break;
    case 0x43: type = L"Old Linux"; break;
    case 0x63: type = L"UNIX"; break;
    case 0x82: type = L"Linux swap, Sun Solaris (old)"; break;
    case 0x83: type = L"Linux"; break;
    case 0x85: type = L"Linux extended"; break;
    case 0x93: type = L"Amoeba, hidden Linux"; break;
    case 0x94: type = L"Amoeba BBT"; break;
    case 0xA5: type = L"Hibernation partition"; break;
    case 0xB6: type = L"Mirror master - FAT-16 Windows NT partition";
    case 0xB7: type = L"Mirror master - NTFS/HPFS Windows NT partition";
    case 0xC2: type = L"Hidden Linux"; break;
    case 0xC3: type = L"Hidden Linux swap"; break;
    case 0xC6: type = L"Mirror slave - FAT-16 Windows NT partition"; break;
    case 0xC7: type = L"Mirror slave - NTFS Windows NT partition"; break;
    case 0xCD: type = L"Memory dump"; break;
    case 0xDA: type = L"Data, not fyle system"; break;
    case 0xDD: type = L"Hidden memory dump"; break;
    case 0xDE: type = L"Dell utility"; break;
    case 0xED: type = L"Hybrid GPT"; break;
    case 0xEE: type = L"GPT"; break;
    case 0xEF: type = L"System UEFI partition"; break;
    default: type = L"unknown";
    }

    size_t length = type.length();
    wchar_t* cType = new wchar_t[length + 1];

    type.copy(cType, length);
    cType[length] = L'\0';

    return cType;
}

LPWSTR asciiToUnicode(char* asciiString) {
    size_t len = strlen(asciiString);
    PWCHAR uniString = new WCHAR[len + 1];
    int result = MultiByteToWideChar(CP_OEMCP, 0, asciiString, -1, uniString, len + 1);
    return uniString;
}

LPWSTR guidToString(byte guid[16]) {
    LPWSTR guidText = new WCHAR[37];
    byte hexPart;
    byte index = 0;

    for (int i = 3; i > -1; i--) {
        hexPart = (guid[i] & 240) >> 4; // 1111 0000 -> 0000 1111
        _itow_s(hexPart, &guidText[index], 2, 16);
        ++index;
        hexPart = guid[i] & 15;  // 0000 1111
        _itow_s(hexPart, &guidText[index], 2, 16);
        ++index;
    }

    guidText[index] = L'-';
    ++index;
    
    for (int i = 4; i < 8; i += 2) {
        hexPart = (guid[i + 1] & 240) >> 4;
        _itow_s(hexPart, &guidText[index], 2, 16);
        ++index;
        hexPart = guid[i + 1] & 15;
        _itow_s(hexPart, &guidText[index], 2, 16);
        ++index;

        hexPart = (guid[i] & 240) >> 4;
        _itow_s(hexPart, &guidText[index], 2, 16);
        ++index;
        hexPart = guid[i] & 15;
        _itow_s(hexPart, &guidText[index], 2, 16);
        ++index;
        guidText[index] = L'-';
        ++index;
    }

    for (int i = 8; i < 16; ++i) {
        hexPart = (guid[i] & 240) >> 4;
        _itow_s(hexPart, &guidText[index], 2, 16);
        ++index;
        hexPart = guid[i] & 15;
        _itow_s(hexPart, &guidText[index], 2, 16);
        ++index;
        if (i == 9) {
            guidText[index] = L'-';
            ++index;
        }
    }

    for (int i = 0; guidText[i] != '\0'; ++i)
        guidText[i] = towupper(guidText[i]);

    return guidText;
}
