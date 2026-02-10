#include "VGMReader.h"
#include "VGMWriter.h"
#include "CommandMapper.h"
#include "VGMValidator.h"
#include <iostream>
#include <string>

class VGMConverterFMOnly {
public:
    VGMConverterFMOnly() : reader(), writer(), mapper(writer) {
    }

    bool Convert(const std::string& inputFile, const std::string& outputFile) {
        std::cout << "=== YM2610 to YM2612 VGM Converter (FM Only) ===" << std::endl;
        std::cout << std::endl;

        // Load input VGM
        std::cout << "Loading input file: " << inputFile << std::endl;
        if (!reader.Load(inputFile)) {
            std::cerr << "Failed to load input file" << std::endl;
            return false;
        }

        const VGMHeader& header = reader.GetHeader();

        // Check if it's a YM2610 VGM
        if (header.ym2610Clock == 0) {
            std::cerr << "Error: Input file does not contain YM2610 data" << std::endl;
            return false;
        }

        std::cout << std::endl;

        // Initialize writer with YM2612 clock (same as YM2610)
        UINT32 ym2612Clock = header.ym2610Clock;
        writer.Initialize(header, ym2612Clock);

        // Convert commands (FM only, skip ADPCM)
        std::cout << "Converting VGM commands (FM only)..." << std::endl;
        if (!ConvertCommands()) {
            std::cerr << "Failed to convert commands" << std::endl;
            return false;
        }

        std::cout << std::endl;

        // Save output VGM
        std::cout << "Saving output file: " << outputFile << std::endl;
        if (!writer.Save(outputFile)) {
            std::cerr << "Failed to save output file" << std::endl;
            return false;
        }

        std::cout << std::endl;
        PrintStatistics();

        // Validate output VGM
        std::cout << "Validating output VGM..." << std::endl;
        VGMValidator validator;
        if (validator.Validate(outputFile)) {
            validator.PrintReport();
        } else {
            std::cerr << "Output VGM validation failed!" << std::endl;
            validator.PrintReport();
            return false;
        }

        return true;
    }

private:
    VGMReader reader;
    VGMWriter writer;
    CommandMapper mapper;

    bool ConvertCommands() {
        const std::vector<UINT8>& data = reader.GetData();
        UINT32 pos = reader.GetDataStart();
        UINT32 dataSize = data.size();

        while (pos < dataSize) {
            UINT8 cmd = data[pos];

            if (cmd == 0x66) {
                // End of data
                writer.WriteCommand(0x66);
                break;
            }
            else if (cmd == 0x67) {
                // Data block - skip (we don't need ADPCM data)
                if (pos + 6 >= dataSize) break;
                UINT32 blockSize = VGMReader::ReadLE32(&data[pos + 3]);
                pos += 7 + blockSize;
            }
            else if (cmd == 0x58) {
                // YM2610 port 0 write
                if (pos + 2 >= dataSize) break;
                UINT8 reg = data[pos + 1];
                UINT8 val = data[pos + 2];
                mapper.ProcessYM2610Port0(reg, val);
                pos += 3;
            }
            else if (cmd == 0x59) {
                // YM2610 port 1 write
                if (pos + 2 >= dataSize) break;
                UINT8 reg = data[pos + 1];
                UINT8 val = data[pos + 2];

                // Only process FM registers, skip ADPCM
                if (reg >= 0x30 || (reg >= 0x20 && reg <= 0x2D)) {
                    // FM register (0x30+ or specific FM registers in lower range)
                    mapper.ProcessYM2610Port1(reg, val);
                }
                // Skip ADPCM control registers (0x00-0x1D, 0x20-0x2D for end addresses)

                pos += 3;
            }
            else if (cmd == 0x61) {
                // Wait N samples
                if (pos + 2 >= dataSize) break;
                UINT16 samples = VGMReader::ReadLE16(&data[pos + 1]);
                writer.WriteCommand(0x61, samples);
                pos += 3;
            }
            else if (cmd == 0x62) {
                // Wait 735 samples (1/60 sec)
                writer.WriteCommand(0x62);
                pos += 1;
            }
            else if (cmd == 0x63) {
                // Wait 882 samples (1/50 sec)
                writer.WriteCommand(0x63);
                pos += 1;
            }
            else if (cmd >= 0x70 && cmd <= 0x7F) {
                // Wait 1-16 samples
                writer.WriteCommand(cmd);
                pos += 1;
            }
            else {
                // Unknown/unsupported command - skip
                UINT32 len = GetCommandLength(cmd, data, pos, dataSize);
                if (len == 0) {
                    std::cerr << "Warning: Unknown command 0x" << std::hex << (int)cmd
                              << " at position 0x" << pos << std::dec << std::endl;
                    break;
                }
                pos += len;
            }
        }

        return true;
    }

    UINT32 GetCommandLength(UINT8 cmd, const std::vector<UINT8>& data, UINT32 pos, UINT32 dataSize) {
        // Return the length of a VGM command
        if (cmd >= 0x70 && cmd <= 0x7F) return 1;  // Wait 1-16 samples
        if (cmd >= 0x80 && cmd <= 0x8F) return 1;  // YM2612 PCM write + wait

        switch (cmd) {
            case 0x50: return 2;  // PSG write
            case 0x51: return 3;  // YM2413
            case 0x52: return 3;  // YM2612 port 0
            case 0x53: return 3;  // YM2612 port 1
            case 0x54: return 3;  // YM2151
            case 0x55: return 3;  // YM2203
            case 0x56: return 3;  // YM2608 port 0
            case 0x57: return 3;  // YM2608 port 1
            case 0x58: return 3;  // YM2610 port 0
            case 0x59: return 3;  // YM2610 port 1
            case 0x61: return 3;  // Wait N samples
            case 0x62: return 1;  // Wait 735 samples
            case 0x63: return 1;  // Wait 882 samples
            case 0x66: return 1;  // End of data
            case 0x67: {         // Data block
                if (pos + 6 >= dataSize) return 0;
                UINT32 blockSize = VGMReader::ReadLE32(&data[pos + 3]);
                return 7 + blockSize;
            }
            case 0xE0: return 5;  // YM2612 PCM seek
            default:
                return 0;  // Unknown
        }
    }

    void PrintStatistics() {
        std::cout << "=== Conversion Statistics ===" << std::endl;
        std::cout << "  FM commands converted: " << mapper.GetFMCommandCount() << std::endl;
        std::cout << "  SSG commands discarded: " << mapper.GetSSGCommandCount() << std::endl;
        std::cout << std::endl;
        std::cout << "Conversion completed successfully!" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    std::string inputFile = "Illusion.vgm";
    std::string outputFile = "Illusion_YM2612_FM_only.vgm";

    if (argc >= 2) {
        inputFile = argv[1];
    }
    if (argc >= 3) {
        outputFile = argv[2];
    }

    VGMConverterFMOnly converter;
    if (!converter.Convert(inputFile, outputFile)) {
        std::cerr << "Conversion failed!" << std::endl;
        return 1;
    }

    return 0;
}
