#include "VGMReader.h"
#include "VGMWriter.h"
#include "CommandMapper.h"
#include "VGMValidator.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <string>

#pragma pack(push, 1)
struct WAVHeader {
    char riff[4];           // "RIFF"
    uint32_t fileSize;      // File size - 8
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t fmtSize;       // Format chunk size
    uint16_t audioFormat;   // Audio format (1 = PCM)
    uint16_t numChannels;   // Number of channels
    uint32_t sampleRate;    // Sample rate
    uint32_t byteRate;      // Byte rate
    uint16_t blockAlign;    // Block align
    uint16_t bitsPerSample; // Bits per sample
    char data[4];           // "data"
    uint32_t dataSize;      // Data size
};
#pragma pack(pop)

class WAVReader {
public:
    bool Load(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open WAV file: " << filename << std::endl;
            return false;
        }

        // Read RIFF header
        char riff[4];
        uint32_t fileSize;
        char wave[4];
        file.read(riff, 4);
        file.read(reinterpret_cast<char*>(&fileSize), 4);
        file.read(wave, 4);

        if (std::memcmp(riff, "RIFF", 4) != 0 || std::memcmp(wave, "WAVE", 4) != 0) {
            std::cerr << "Invalid WAV file" << std::endl;
            return false;
        }

        // Read fmt chunk
        char fmt[4];
        uint32_t fmtSize;
        file.read(fmt, 4);
        file.read(reinterpret_cast<char*>(&fmtSize), 4);

        if (std::memcmp(fmt, "fmt ", 4) != 0) {
            std::cerr << "Invalid fmt chunk" << std::endl;
            return false;
        }

        // Read format data
        file.read(reinterpret_cast<char*>(&audioFormat), 2);
        file.read(reinterpret_cast<char*>(&numChannels), 2);
        file.read(reinterpret_cast<char*>(&sampleRate), 4);
        file.read(reinterpret_cast<char*>(&byteRate), 4);
        file.read(reinterpret_cast<char*>(&blockAlign), 2);
        file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

        // Skip any extra format bytes
        if (fmtSize > 16) {
            file.seekg(fmtSize - 16, std::ios::cur);
        }

        // Find data chunk
        char chunkId[4];
        uint32_t chunkSize;
        bool foundData = false;

        while (file.read(chunkId, 4)) {
            file.read(reinterpret_cast<char*>(&chunkSize), 4);

            if (std::memcmp(chunkId, "data", 4) == 0) {
                foundData = true;
                break;
            }

            // Skip this chunk
            file.seekg(chunkSize, std::ios::cur);
        }

        if (!foundData) {
            std::cerr << "No data chunk found" << std::endl;
            return false;
        }

        // Read samples
        uint32_t numSamples = chunkSize / sizeof(int16_t);
        samples.resize(numSamples);
        file.read(reinterpret_cast<char*>(samples.data()), chunkSize);

        std::cout << "Loaded WAV file:" << std::endl;
        std::cout << "  Sample rate: " << sampleRate << " Hz" << std::endl;
        std::cout << "  Channels: " << numChannels << std::endl;
        std::cout << "  Bits per sample: " << bitsPerSample << std::endl;
        std::cout << "  Samples: " << numSamples << " (" << numSamples / numChannels << " frames)" << std::endl;

        return true;
    }

    uint16_t GetAudioFormat() const { return audioFormat; }
    uint16_t GetNumChannels() const { return numChannels; }
    uint32_t GetSampleRate() const { return sampleRate; }
    uint16_t GetBitsPerSample() const { return bitsPerSample; }
    const std::vector<int16_t>& GetSamples() const { return samples; }

private:
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    std::vector<int16_t> samples;
};

class VGMConverterWithDAC {
public:
    VGMConverterWithDAC() : reader(), writer(), mapper(writer) {
    }

    bool Convert(const std::string& inputVGM, const std::string& inputWAV, const std::string& outputFile) {
        std::cout << "=== YM2610 to YM2612 VGM Converter with DAC ===" << std::endl;
        std::cout << std::endl;

        // Load input VGM
        std::cout << "Loading VGM file: " << inputVGM << std::endl;
        if (!reader.Load(inputVGM)) {
            std::cerr << "Failed to load VGM file" << std::endl;
            return false;
        }

        const VGMHeader& header = reader.GetHeader();

        // Check if it's a YM2610 VGM
        if (header.ym2610Clock == 0) {
            std::cerr << "Error: Input file does not contain YM2610 data" << std::endl;
            return false;
        }

        std::cout << std::endl;

        // Load ADPCM WAV
        std::cout << "Loading ADPCM WAV file: " << inputWAV << std::endl;
        WAVReader wavReader;
        if (!wavReader.Load(inputWAV)) {
            std::cerr << "Failed to load WAV file" << std::endl;
            return false;
        }

        std::cout << std::endl;

        // Prepare DAC samples
        if (!PrepareDACData(wavReader)) {
            std::cerr << "Failed to prepare DAC data" << std::endl;
            return false;
        }

        std::cout << std::endl;

        // Initialize writer with YM2612 clock (same as YM2610)
        UINT32 ym2612Clock = header.ym2610Clock;
        writer.Initialize(header, ym2612Clock);

        // Copy GD3 tag data
        std::vector<UINT8> gd3Data = reader.GetGD3Data();
        if (!gd3Data.empty()) {
            writer.SetGD3Data(gd3Data);
            std::cout << "Copied GD3 tag (" << gd3Data.size() << " bytes)" << std::endl;
        }

        // Convert commands with DAC
        std::cout << "Converting VGM commands with DAC..." << std::endl;
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
    std::vector<UINT8> dacSamples;  // Mono 8-bit unsigned samples
    UINT32 dacSampleIndex;
    UINT32 dacSampleRate;
    UINT32 vgmSampleRate;  // VGM sample rate (44100)
    double dacAccumulator;  // Accumulator for fractional DAC samples
    UINT8 lastDacSample;  // Track last written sample to skip redundant writes

    bool PrepareDACData(const WAVReader& wavReader) {
        const std::vector<int16_t>& samples = wavReader.GetSamples();
        UINT16 numChannels = wavReader.GetNumChannels();
        dacSampleRate = wavReader.GetSampleRate();
        vgmSampleRate = 44100;  // VGM standard sample rate

        std::cout << "Preparing DAC data..." << std::endl;

        // Convert to mono 8-bit unsigned
        UINT32 numFrames = samples.size() / numChannels;
        dacSamples.resize(numFrames);

        for (UINT32 i = 0; i < numFrames; i++) {
            // Mix channels to mono
            int32_t sum = 0;
            for (UINT16 ch = 0; ch < numChannels; ch++) {
                sum += samples[i * numChannels + ch];
            }
            int16_t mono = sum / numChannels;

            // Convert to 8-bit unsigned (0-255)
            dacSamples[i] = static_cast<UINT8>((mono + 32768) >> 8);
        }

        std::cout << "  Prepared " << dacSamples.size() << " DAC samples" << std::endl;
        std::cout << "  DAC sample rate: " << dacSampleRate << " Hz" << std::endl;
        std::cout << "  VGM sample rate: " << vgmSampleRate << " Hz" << std::endl;
        std::cout << "  Sample rate ratio: " << (double)vgmSampleRate / dacSampleRate << "x" << std::endl;

        dacSampleIndex = 0;
        dacAccumulator = 0.0;
        lastDacSample = 0x80;  // Initialize to silence (center value)

        return true;
    }

    void WriteDACForSamples(UINT32 vgmSamples) {
        // Write DAC samples with proper timing, handling sample rate conversion
        // dacSampleRate (e.g., 22050) -> vgmSampleRate (44100)
        // Ratio = vgmSampleRate / dacSampleRate (e.g., 2.0)

        double ratio = (double)vgmSampleRate / dacSampleRate;

        for (UINT32 i = 0; i < vgmSamples; i++) {
            // Calculate which DAC sample to use
            dacAccumulator += 1.0 / ratio;
            UINT32 targetIndex = (UINT32)dacAccumulator;

            if (targetIndex >= dacSamples.size()) {
                // Reached end of DAC samples, write silence
                writer.WriteCommand(0x52, 0x2A, 0x80);
            } else {
                UINT8 sample = dacSamples[targetIndex];
                writer.WriteCommand(0x52, 0x2A, sample);
                lastDacSample = sample;
            }

            // Write wait for 1 VGM sample after each DAC write
            writer.WriteCommand(0x70);  // 0x70 = wait 1 sample
        }

        // Update dacSampleIndex to track progress
        dacSampleIndex = (UINT32)dacAccumulator;
    }

    bool ConvertCommands() {
        const std::vector<UINT8>& data = reader.GetData();
        UINT32 pos = reader.GetDataStart();
        UINT32 dataSize = data.size();
        const VGMHeader& header = reader.GetHeader();

        // Calculate loop position in source VGM
        UINT32 loopPos = 0;
        if (header.loopOffset > 0) {
            loopPos = 0x1C + header.loopOffset;
        }

        // Enable DAC: write 0x80 to register 0x2B
        writer.WriteCommand(0x52, 0x2B, 0x80);

        // Set channel 6 (FM channel 5, index 2 in port 1) pan to both speakers
        // Register 0xB6 (0xB4 + channel 2): bits 7-6 = L/R enable
        // 0xC0 = both left and right enabled
        writer.WriteCommand(0x53, 0xB6, 0xC0);

        while (pos < dataSize) {
            // Check if we've reached the loop point
            if (loopPos > 0 && pos == loopPos) {
                writer.MarkLoopPoint();
            }

            UINT8 cmd = data[pos];

            if (cmd == 0x66) {
                // End of data
                writer.WriteCommand(0x66);
                break;
            }
            else if (cmd == 0x67) {
                // Data block - skip (we don't need ADPCM data blocks)
                if (pos + 6 >= dataSize) break;
                UINT32 blockSize = VGMReader::ReadLE32(&data[pos + 3]);
                pos += 7 + blockSize;
            }
            else if (cmd == 0x58) {
                // YM2610 port 0 write
                if (pos + 2 >= dataSize) break;
                UINT8 reg = data[pos + 1];
                UINT8 val = data[pos + 2];

                // Remap FM channel 6 to FM channel 4 (v2.3 mapping)
                // Key on/off register 0x28: bits 0-2 = channel
                if (reg == 0x28) {
                    UINT8 channel = val & 0x07;
                    if (channel == 6) {
                        // Remap FM6 (channel 6) to FM4 (channel 4)
                        val = (val & 0xF8) | 4;
                    }
                }

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
                    // Remap FM channel 6 (channel 2 in port 1) to FM channel 4 (channel 0 in port 1)
                    // Channel-specific registers: 0x30-0xB6
                    if (reg >= 0x30 && reg <= 0xB6) {
                        UINT8 channel_offset = reg & 0x03;
                        if (channel_offset == 2) {
                            // Remap FM6 to FM4 (channel 0 in port 1)
                            reg = (reg & 0xFC) | 0;
                        }
                    }
                    mapper.ProcessYM2610Port1(reg, val);
                }

                pos += 3;
            }
            else if (cmd == 0x61) {
                // Wait N samples
                if (pos + 2 >= dataSize) break;
                UINT16 samples = VGMReader::ReadLE16(&data[pos + 1]);

                // Write DAC samples for this delay (includes wait commands)
                WriteDACForSamples(samples);

                // Don't write the original wait command - it's already included in WriteDACForSamples
                pos += 3;
            }
            else if (cmd == 0x62) {
                // Wait 735 samples (1/60 sec)
                WriteDACForSamples(735);
                // Don't write the original wait command
                pos += 1;
            }
            else if (cmd == 0x63) {
                // Wait 882 samples (1/50 sec)
                WriteDACForSamples(882);
                // Don't write the original wait command
                pos += 1;
            }
            else if (cmd >= 0x70 && cmd <= 0x7F) {
                // Wait 1-16 samples
                UINT8 waitSamples = (cmd & 0x0F) + 1;
                WriteDACForSamples(waitSamples);
                // Don't write the original wait command
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

        std::cout << "  Wrote " << dacSampleIndex << " / " << dacSamples.size() << " DAC samples" << std::endl;

        return true;
    }

    UINT32 GetCommandLength(UINT8 cmd, const std::vector<UINT8>& data, UINT32 pos, UINT32 dataSize) {
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
                if (pos + 6 >= dataSize) return 0;
                UINT32 blockSize = VGMReader::ReadLE32(&data[pos + 3]);
                return 7 + blockSize;
            }
            case 0xE0: return 5;
            default:
                return 0;
        }
    }

    void PrintStatistics() {
        std::cout << "=== Conversion Statistics ===" << std::endl;
        std::cout << "  FM commands converted: " << mapper.GetFMCommandCount() << std::endl;
        std::cout << "  SSG commands discarded: " << mapper.GetSSGCommandCount() << std::endl;
        std::cout << "  DAC samples written: " << dacSampleIndex << std::endl;
        std::cout << std::endl;
        std::cout << "Conversion completed successfully!" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: vgm_converter_with_dac <input.vgm> <adpcm.wav> [output.vgm]" << std::endl;
        std::cout << "  Converts YM2610 VGM to YM2612 VGM with ADPCM as DAC" << std::endl;
        return 1;
    }

    std::string inputVGM = argv[1];
    std::string inputWAV = argv[2];
    std::string outputFile = "output_with_dac.vgm";

    if (argc >= 4) {
        outputFile = argv[3];
    }

    VGMConverterWithDAC converter;
    if (!converter.Convert(inputVGM, inputWAV, outputFile)) {
        std::cerr << "Conversion failed!" << std::endl;
        return 1;
    }

    return 0;
}
