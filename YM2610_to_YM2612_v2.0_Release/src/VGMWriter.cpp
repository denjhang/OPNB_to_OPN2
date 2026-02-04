#include "VGMWriter.h"
#include <fstream>
#include <cstring>
#include <iostream>

VGMWriter::VGMWriter() {
    memset(&header, 0, sizeof(header));
    loopCommandOffset = 0;
    hasLoopPoint = false;
}

VGMWriter::~VGMWriter() {
}

void VGMWriter::Initialize(const VGMHeader& sourceHeader, UINT32 ym2612Clock) {
    // Copy source header
    header = sourceHeader;

    // Clear YM2610 clock
    header.ym2610Clock = 0;

    // Set YM2612 clock
    header.ym2612Clock = ym2612Clock;

    // Clear SSG-related clocks
    header.ay8910Clock = 0;

    // Reset offsets (will be calculated when saving)
    header.eofOffset = 0;
    header.gd3Offset = 0;
    header.loopOffset = 0;
    header.dataOffset = 0x0C; // Standard offset for v1.50+

    std::cout << "VGMWriter initialized:" << std::endl;
    std::cout << "  YM2612 clock: " << ym2612Clock << " Hz" << std::endl;
}

void VGMWriter::WriteCommand(UINT8 cmd) {
    commandData.push_back(cmd);
}

void VGMWriter::WriteCommand(UINT8 cmd, UINT8 data1) {
    commandData.push_back(cmd);
    commandData.push_back(data1);
}

void VGMWriter::WriteCommand(UINT8 cmd, UINT8 data1, UINT8 data2) {
    commandData.push_back(cmd);
    commandData.push_back(data1);
    commandData.push_back(data2);
}

void VGMWriter::WriteCommand(UINT8 cmd, UINT16 data) {
    commandData.push_back(cmd);
    commandData.push_back(data & 0xFF);
    commandData.push_back((data >> 8) & 0xFF);
}

void VGMWriter::MarkLoopPoint() {
    loopCommandOffset = commandData.size() + dataBlocks.size();
    hasLoopPoint = true;
}

void VGMWriter::SetGD3Data(const std::vector<UINT8>& data) {
    gd3Data = data;
}

void VGMWriter::WriteDataBlock(UINT8 type, const std::vector<UINT8>& blockData) {
    dataBlocks.push_back(0x67);  // Data block command
    dataBlocks.push_back(0x66);  // Compatibility command
    dataBlocks.push_back(type);  // Block type

    // Block size (4 bytes, little-endian)
    UINT32 size = blockData.size();
    dataBlocks.push_back(size & 0xFF);
    dataBlocks.push_back((size >> 8) & 0xFF);
    dataBlocks.push_back((size >> 16) & 0xFF);
    dataBlocks.push_back((size >> 24) & 0xFF);

    // Block data
    dataBlocks.insert(dataBlocks.end(), blockData.begin(), blockData.end());
}

bool VGMWriter::Save(const std::string& filename) {
    std::vector<UINT8> output;

    // Reserve space for header (256 bytes to be safe)
    output.resize(256, 0);

    // Write VGM signature
    WriteLE32(output, 0x00, 0x206D6756); // "Vgm "

    // Write version
    WriteLE32(output, 0x08, header.version);

    // Write clocks
    WriteLE32(output, 0x0C, header.sn76489Clock);
    WriteLE32(output, 0x10, header.ym2413Clock);
    WriteLE32(output, 0x2C, header.ym2612Clock);
    WriteLE32(output, 0x30, header.ym2151Clock);
    WriteLE32(output, 0x38, header.segaPcmClock);
    WriteLE32(output, 0x40, header.rf5c68Clock);
    WriteLE32(output, 0x44, header.ym2203Clock);
    WriteLE32(output, 0x48, header.ym2608Clock);
    WriteLE32(output, 0x4C, 0); // YM2610 clock = 0

    // Write total samples
    WriteLE32(output, 0x18, header.totalSamples);

    // Write loop samples
    WriteLE32(output, 0x20, header.loopSamples);

    // Write rate
    WriteLE32(output, 0x24, header.rate);

    // Write SN76489 config
    WriteLE16(output, 0x28, header.sn76489Feedback);
    output[0x2A] = header.sn76489ShiftWidth;
    output[0x2B] = header.sn76489Flags;

    // Write flags
    output[0x78] = header.ay8910Type;
    output[0x79] = header.ay8910Flags;
    output[0x7A] = header.ym2203Flags;
    output[0x7B] = header.ym2608Flags;
    output[0x7C] = header.volumeModifier;
    output[0x7E] = header.loopBase;
    output[0x7F] = header.loopModifier;

    // Data offset (relative to 0x34)
    UINT32 dataStart = 0x100; // Start data at offset 0x100
    WriteLE32(output, 0x34, dataStart - 0x34);

    // Resize to data start
    output.resize(dataStart);

    // Append data blocks
    UINT32 dataBlocksStart = output.size();
    output.insert(output.end(), dataBlocks.begin(), dataBlocks.end());

    // Append command data
    output.insert(output.end(), commandData.begin(), commandData.end());

    // Calculate and write EOF offset (relative to 0x04)
    UINT32 eofOffset = output.size() - 0x04;
    WriteLE32(output, 0x04, eofOffset);

    // Write loop offset if present
    if (hasLoopPoint && header.loopSamples > 0) {
        // Loop offset is relative to 0x1C
        UINT32 loopOffset = dataStart + loopCommandOffset - 0x1C;
        WriteLE32(output, 0x1C, loopOffset);
    } else {
        WriteLE32(output, 0x1C, 0);
    }

    // Append GD3 tag if present
    if (!gd3Data.empty()) {
        // GD3 offset is relative to 0x14
        UINT32 gd3Offset = output.size() - 0x14;
        WriteLE32(output, 0x14, gd3Offset);
        output.insert(output.end(), gd3Data.begin(), gd3Data.end());
    } else {
        WriteLE32(output, 0x14, 0);
    }

    // Write to file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to create output file: " << filename << std::endl;
        return false;
    }

    file.write((char*)output.data(), output.size());
    file.close();

    std::cout << "VGM saved successfully: " << filename << std::endl;
    std::cout << "  Output size: " << output.size() << " bytes" << std::endl;
    std::cout << "  Command data: " << commandData.size() << " bytes" << std::endl;
    std::cout << "  Data blocks: " << dataBlocks.size() << " bytes" << std::endl;

    return true;
}

void VGMWriter::WriteLE32(std::vector<UINT8>& data, UINT32 offset, UINT32 value) {
    if (offset + 3 >= data.size()) {
        data.resize(offset + 4);
    }
    data[offset] = value & 0xFF;
    data[offset + 1] = (value >> 8) & 0xFF;
    data[offset + 2] = (value >> 16) & 0xFF;
    data[offset + 3] = (value >> 24) & 0xFF;
}

void VGMWriter::WriteLE16(std::vector<UINT8>& data, UINT32 offset, UINT16 value) {
    if (offset + 1 >= data.size()) {
        data.resize(offset + 2);
    }
    data[offset] = value & 0xFF;
    data[offset + 1] = (value >> 8) & 0xFF;
}
