#pragma once
#include "pch.h"

namespace MBR {
    // DON'T REPLACE BYTE[4] ARRAYS TO INT!
    // IN THIS CASE COMPILER USE ADDRESS ALIGNMENT AND THAT MAKES STRUCTURE SIZE IS NOT EQUALS 512 BYTES!!!
    struct partition {
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
        partition partitions[4];
        byte endSignature[2];
    };

    bool isGptDisk(MBR::mbr mbr);
    void print(MBR::partition part, bool fullInfo = false);
    void print(MBR::mbr mbr, bool fullInfo = false);
    //std::string typeCodeToString(int partTypeCode);
}