#include "VGMValidator.h"
#include "VGMReader.h"
#include <fstream>
#include <iostream>
#include <iomanip>

VGMValidator::VGMValidator() {
    result.valid = false;
    result.fileSize = 0;
    result.version = 0;
    result.totalSamples = 0;
    result.commandCount = 0;
    result.dataBlockCount = 0;
    result.hasYM2612 = false;
    result.ym2612Clock = 0;
}

VGMValidator::~VGMValidator() {
}

bool VGMValidator::Validate(const std::string& filename) {
    result.errors.clear();
    result.warnings.clear();

    // Load file
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        result.errors.push_back("Failed to open file: " + filename);
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<UINT8> data(size);
    if (!file.read((char*)data.data(), size)) {
        result.errors.push_back("Failed to read file");
        return false;
    }

    result.fileSize = size;

    // Validate header
    if (!ValidateHeader(data)) {
        return false;
    }

    // Calculate data start
    UINT32 dataStart = 0x40;
    if (result.version >= 0x150) {
        UINT32 dataOffset = VGMReader::ReadLE32(&data[0x34]);
        dataStart = 0x34 + dataOffset;
    }

    // Validate commands
    if (!ValidateCommands(data, dataStart)) {
        return false;
    }

    result.valid = true;
    return true;
}

bool VGMValidator::ValidateHeader(const std::vector<UINT8>& data) {
    if (data.size() < 0x40) {
        result.errors.push_back("File too small (< 64 bytes)");
        return false;
    }

    // Check signature
    UINT32 ident = VGMReader::ReadLE32(&data[0]);
    if (ident != 0x206D6756) { // "Vgm " in little-endian
        result.errors.push_back("Invalid VGM signature");
        return false;
    }

    // Read version
    result.version = VGMReader::ReadLE32(&data[0x08]);
    if (result.version < 0x150) {
        result.warnings.push_back("Old VGM version (< 1.50)");
    }

    // Read EOF offset
    UINT32 eofOffset = VGMReader::ReadLE32(&data[0x04]);
    UINT32 expectedSize = eofOffset + 0x04;
    if (expectedSize != data.size()) {
        result.warnings.push_back("EOF offset mismatch: expected " +
                                  std::to_string(expectedSize) + ", got " +
                                  std::to_string(data.size()));
    }

    // Read total samples
    result.totalSamples = VGMReader::ReadLE32(&data[0x18]);

    // Check YM2612 clock
    if (data.size() >= 0x30) {
        result.ym2612Clock = VGMReader::ReadLE32(&data[0x2C]);
        result.hasYM2612 = (result.ym2612Clock != 0);
    }

    if (!result.hasYM2612) {
        result.errors.push_back("No YM2612 chip found in VGM");
        return false;
    }

    return true;
}

bool VGMValidator::ValidateCommands(const std::vector<UINT8>& data, UINT32 dataStart) {
    UINT32 pos = dataStart;
    bool foundEnd = false;

    while (pos < data.size()) {
        UINT8 cmd = data[pos];

        if (cmd == 0x66) {
            // End of data
            foundEnd = true;
            result.commandCount++;
            break;
        }

        UINT32 cmdLen = GetCommandLength(cmd, data, pos);
        if (cmdLen == 0) {
            result.errors.push_back("Unknown command 0x" +
                                   std::to_string(cmd) + " at offset 0x" +
                                   std::to_string(pos));
            return false;
        }

        if (pos + cmdLen > data.size()) {
            result.errors.push_back("Command extends beyond file at offset 0x" +
                                   std::to_string(pos));
            return false;
        }

        // Count data blocks
        if (cmd == 0x67) {
            result.dataBlockCount++;
        }

        result.commandCount++;
        pos += cmdLen;
    }

    if (!foundEnd) {
        result.errors.push_back("No end-of-data command (0x66) found");
        return false;
    }

    return true;
}

UINT32 VGMValidator::GetCommandLength(UINT8 cmd, const std::vector<UINT8>& data, UINT32 pos) {
    if (cmd >= 0x70 && cmd <= 0x7F) return 1;
    if (cmd >= 0x80 && cmd <= 0x8F) return 1;

    switch (cmd) {
        case 0x50: return 2;
        case 0x51: return 3;
        case 0x52: return 3;
        case 0x53: return 3;
        case 0x54: return 3;
        case 0x55: return 3;
        case 0x56: return 3;
        case 0x57: return 3;
        case 0x58: return 3;
        case 0x59: return 3;
        case 0x61: return 3;
        case 0x62: return 1;
        case 0x63: return 1;
        case 0x66: return 1;
        case 0x67: {
            if (pos + 6 >= data.size()) return 0;
            UINT32 blockSize = VGMReader::ReadLE32(&data[pos + 3]);
            return 7 + blockSize;
        }
        case 0xE0: return 5;
        default: return 0;
    }
}

void VGMValidator::PrintReport() const {
    std::cout << std::endl;
    std::cout << "=== VGM Validation Report ===" << std::endl;
    std::cout << std::endl;

    if (result.valid) {
        std::cout << "✓ VGM file is VALID" << std::endl;
    } else {
        std::cout << "✗ VGM file is INVALID" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "File Statistics:" << std::endl;
    std::cout << "  File size: " << result.fileSize << " bytes" << std::endl;
    std::cout << "  Version: " << std::hex << result.version << std::dec << std::endl;
    std::cout << "  Total samples: " << result.totalSamples << std::endl;
    std::cout << "  Commands: " << result.commandCount << std::endl;
    std::cout << "  Data blocks: " << result.dataBlockCount << std::endl;

    if (result.hasYM2612) {
        std::cout << "  YM2612 clock: " << result.ym2612Clock << " Hz" << std::endl;
    }

    if (!result.errors.empty()) {
        std::cout << std::endl;
        std::cout << "Errors (" << result.errors.size() << "):" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  ✗ " << error << std::endl;
        }
    }

    if (!result.warnings.empty()) {
        std::cout << std::endl;
        std::cout << "Warnings (" << result.warnings.size() << "):" << std::endl;
        for (const auto& warning : result.warnings) {
            std::cout << "  ⚠ " << warning << std::endl;
        }
    }

    std::cout << std::endl;
}
