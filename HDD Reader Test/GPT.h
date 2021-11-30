#pragma once
#include "pch.h"

namespace GPT {
	struct gptHeader {
		BYTE signature1[8];
		BYTE headerVersion2[4];
		int headerSize3;
		int headerCrc32Hash4;
		int firstReserved5;
		byte primaryGptHeaderAddress6[8];
		byte secondaryGptHeaderAddress7[8];
		byte firstPartitionAddress8[8];
		byte lastPartitionSectorOfDisk9[8];
		byte diskGuid10[16];
		byte partitionTableAddress11[8];
		int maxPartitionAmount12;
		int partitionRecordSize13;
		int partTableCrc32Hash14;
		BYTE secondReserved15[420];
	};

	struct partition {
		byte typeGuid[16];
		byte partitionGuid[16];
		byte firstLbaAddress[8];
		byte lastLbaAddress[8];
		byte attributes[8];
		WCHAR partitionName[36];
	};

	struct gpt {
		MBR::mbr pmbr;
		gptHeader header;
		partition partitionTable[128];
	};

	void print(GPT::gpt* gpt);
	void print(GPT::partition* part);
}

