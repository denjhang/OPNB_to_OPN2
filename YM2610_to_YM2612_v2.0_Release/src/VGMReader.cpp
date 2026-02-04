#include "VGMReader.h"
#include <fstream>
#include <cstring>
#include <iostream>

VGMReader::VGMReader() : valid(false), dataStart(0) {
    memset(&header, 0, sizeof(header));
}

VGMReader::~VGMReader() {
}

bool VGMReader::Load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read entire file
    data.resize(size);
    if (!file.read((char*)data.data(), size)) {
        std::cerr << "Failed to read file: " << filename << std::endl;
        return false;
    }

    // Validate VGM signature
    if (size < 0x40) {
        std::cerr << "File too small to be a valid VGM" << std::endl;
        return false;
    }

    UINT32 ident = ReadLE32(&data[0]);
    if (ident != 0x206D6756) { // "Vgm " in little-endian
        std::cerr << "Invalid VGM signature" << std::endl;
        return false;
    }

    // Parse header
    memcpy(&header, data.data(), sizeof(VGMHeader));

    // Calculate data start offset
    if (header.version >= 0x150) {
        dataStart = 0x34 + header.dataOffset;
    } else {
        dataStart = 0x40;
    }

    if (dataStart >= data.size()) {
        std::cerr << "Invalid data offset" << std::endl;
        return false;
    }

    valid = true;
    std::cout << "VGM loaded successfully:" << std::endl;
    std::cout << "  Version: " << std::hex << header.version << std::dec << std::endl;
    std::cout << "  Total samples: " << header.totalSamples << std::endl;
    std::cout << "  YM2610 clock: " << header.ym2610Clock << " Hz" << std::endl;
    std::cout << "  Data start: 0x" << std::hex << dataStart << std::dec << std::endl;

    return true;
}

UINT32 VGMReader::ReadLE32(const UINT8* data) {
    return data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}

UINT16 VGMReader::ReadLE16(const UINT8* data) {
    return data[0] | (data[1] << 8);
}

std::vector<UINT8> VGMReader::GetGD3Data() const {
    std::vector<UINT8> gd3Data;

    if (!valid || header.gd3Offset == 0) {
        return gd3Data;  // No GD3 tag
    }

    // Calculate GD3 position
    UINT32 gd3Pos = 0x14 + header.gd3Offset;

    if (gd3Pos + 12 > data.size()) {
        return gd3Data;  // Invalid GD3 offset
    }

    // Verify GD3 signature
    UINT32 gd3Ident = ReadLE32(&data[gd3Pos]);
    if (gd3Ident != 0x20336447) {  // "Gd3 " in little-endian
        return gd3Data;  // Invalid GD3 signature
    }

    // Read GD3 size
    UINT32 gd3Size = ReadLE32(&data[gd3Pos + 8]);

    // Copy entire GD3 tag (header + data)
    UINT32 totalSize = 12 + gd3Size;
    if (gd3Pos + totalSize > data.size()) {
        return gd3Data;  // Invalid GD3 size
    }

    gd3Data.resize(totalSize);
    memcpy(gd3Data.data(), &data[gd3Pos], totalSize);

    return gd3Data;
}
