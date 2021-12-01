#pragma once
#include "pch.h"

namespace ptable {
	// DON'T REPLACE BYTE[4] ARRAYS TO INT!
	// IN THIS CASE COMPILER USE ADDRESS ALIGNMENT AND THAT MAKES STRUCTURE SIZE IS NOT EQUALS 512 BYTES!!!
	struct mbrPartitionRecord {
		byte isActive;
		byte beginHead;
		byte beginSectorCylinder[2]; // bits 0-5 are sector, bits 6-7 are head
		byte type;
		byte endHead;
		byte endSectorCylinder[2]; // bits 0-5 are sector, bits 6-7 are head
		BYTE firstSectorOffset[4];
		BYTE partitionSectorAmount[4];
	};

	struct mbr {
		byte loader[446];
		mbrPartitionRecord partitions[4];
		byte endSignature[2];
	};

	struct gptHeader {
		BYTE signature[8];
		BYTE headerVersion[4];
		int headerSize;
		int headerCrc32Hash;
		int firstReserved;
		byte primaryGptHeaderAddress[8];
		byte secondaryGptHeaderAddress[8];
		byte firstPartitionAddress[8];
		byte lastPartitionSectorOfDisk[8];
		byte diskGuid[16];
		byte partitionTableAddress[8];
		int maxPartitionAmount;
		int partitionRecordSize;
		int partTableCrc32Hash;
		BYTE secondReserved[420];
	};

	struct gptPartitionRecord {
		byte typeGuid[16];
		byte partitionGuid[16];
		byte firstLbaAddress[8];
		byte lastLbaAddress[8];
		byte attributes[8];
		WCHAR partitionName[36];
	};

	struct gpt {
		mbr pmbr;
		gptHeader header;
		gptPartitionRecord partitionTable[128];
	};

	bool isGptDisk(ptable::mbr mbr);
	void print(ptable::mbrPartitionRecord part, bool fullInfo = false);
	void print(ptable::mbr mbr, bool fullInfo = false);
	//std::string typeCodeToString(int partTypeCode);
	void print(gpt* gpt);
	void print(gptPartitionRecord* part);
}
