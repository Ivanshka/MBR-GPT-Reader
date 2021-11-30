#include "pch.h"
#include "mbr.h"

wchar_t* typeCodeToWstring(byte partTypeCode) {
	std::wstring type;
	switch (partTypeCode) {
	// https://ru.wikipedia.org/wiki/Главная_загрузочная_запись#Код_типа_раздела
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
	//case 0x: type = "";
	default: type = L"unknown";
	}
	size_t length = type.length();
	wchar_t* cType = new wchar_t[length+1];
	type.copy(cType, length);
	cType[length] = L'\0';
	return cType;
}

bool MBR::isGptDisk(MBR::mbr mbr)
{
	return mbr.partitions[0].type == 0xEE;
}

void MBR::print(MBR::partition part, bool fullInfo)
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

void MBR::print(MBR::mbr mbr, bool fullInfo)
{
	if (mbr.partitions[0].type == 0xEE)
	{
		wprintf(L"Disk type is GPT so disk has PMBR (protected MBR) with 1 partition!\n");
		print(mbr.partitions[0]);
		return;
	}
	for (int i = 0; i < 4; i++) {
		wprintf(L"Partition %d\n", i);
		print(mbr.partitions[i], fullInfo);
	}
}
