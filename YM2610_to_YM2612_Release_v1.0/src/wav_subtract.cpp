#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <algorithm>

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

bool ReadWAV(const std::string& filename, WAVHeader& header, std::vector<int16_t>& samples) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
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
        std::cerr << "Invalid WAV file: " << filename << std::endl;
        return false;
    }

    // Read fmt chunk
    char fmt[4];
    uint32_t fmtSize;
    file.read(fmt, 4);
    file.read(reinterpret_cast<char*>(&fmtSize), 4);

    if (std::memcmp(fmt, "fmt ", 4) != 0) {
        std::cerr << "Invalid fmt chunk: " << filename << std::endl;
        return false;
    }

    // Read format data
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;

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
        std::cerr << "No data chunk found: " << filename << std::endl;
        return false;
    }

    // Fill header structure
    std::memcpy(header.riff, "RIFF", 4);
    header.fileSize = fileSize;
    std::memcpy(header.wave, "WAVE", 4);
    std::memcpy(header.fmt, "fmt ", 4);
    header.fmtSize = 16;  // Standard size for output
    header.audioFormat = audioFormat;
    header.numChannels = numChannels;
    header.sampleRate = sampleRate;
    header.byteRate = byteRate;
    header.blockAlign = blockAlign;
    header.bitsPerSample = bitsPerSample;
    std::memcpy(header.data, "data", 4);
    header.dataSize = chunkSize;

    // Read samples
    uint32_t numSamples = chunkSize / sizeof(int16_t);
    samples.resize(numSamples);
    file.read(reinterpret_cast<char*>(samples.data()), chunkSize);

    std::cout << "Read " << filename << ":" << std::endl;
    std::cout << "  Sample rate: " << sampleRate << " Hz" << std::endl;
    std::cout << "  Channels: " << numChannels << std::endl;
    std::cout << "  Bits per sample: " << bitsPerSample << std::endl;
    std::cout << "  Samples: " << numSamples << " (" << numSamples / numChannels << " frames)" << std::endl;

    return true;
}

bool WriteWAV(const std::string& filename, const WAVHeader& header, const std::vector<int16_t>& samples) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to create file: " << filename << std::endl;
        return false;
    }

    // Write header
    file.write(reinterpret_cast<const char*>(&header), sizeof(WAVHeader));

    // Write samples
    file.write(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(int16_t));

    std::cout << "Wrote " << filename << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Usage: wav_subtract <full.wav> <subtract.wav> <output.wav>" << std::endl;
        std::cout << "  Subtracts subtract.wav from full.wav to produce output.wav" << std::endl;
        return 1;
    }

    std::string fullFile = argv[1];
    std::string subtractFile = argv[2];
    std::string outputFile = argv[3];

    std::cout << "=== WAV Subtraction Tool ===" << std::endl;
    std::cout << std::endl;

    // Read full audio
    WAVHeader fullHeader;
    std::vector<int16_t> fullSamples;
    if (!ReadWAV(fullFile, fullHeader, fullSamples)) {
        return 1;
    }

    std::cout << std::endl;

    // Read audio to subtract
    WAVHeader subtractHeader;
    std::vector<int16_t> subtractSamples;
    if (!ReadWAV(subtractFile, subtractHeader, subtractSamples)) {
        return 1;
    }

    std::cout << std::endl;

    // Validate compatibility
    if (fullHeader.sampleRate != subtractHeader.sampleRate) {
        std::cerr << "Error: Sample rates don't match!" << std::endl;
        return 1;
    }

    if (fullHeader.numChannels != subtractHeader.numChannels) {
        std::cerr << "Error: Channel counts don't match!" << std::endl;
        return 1;
    }

    if (fullHeader.bitsPerSample != subtractHeader.bitsPerSample) {
        std::cerr << "Error: Bits per sample don't match!" << std::endl;
        return 1;
    }

    // Perform subtraction
    std::cout << "Performing subtraction..." << std::endl;

    size_t minSamples = std::min(fullSamples.size(), subtractSamples.size());
    std::vector<int16_t> resultSamples(minSamples);

    for (size_t i = 0; i < minSamples; i++) {
        int32_t diff = static_cast<int32_t>(fullSamples[i]) - static_cast<int32_t>(subtractSamples[i]);

        // Clamp to int16_t range
        if (diff > 32767) diff = 32767;
        if (diff < -32768) diff = -32768;

        resultSamples[i] = static_cast<int16_t>(diff);
    }

    std::cout << "  Processed " << minSamples << " samples" << std::endl;

    // Calculate statistics
    int64_t sumAbs = 0;
    int16_t maxAbs = 0;
    for (size_t i = 0; i < resultSamples.size(); i++) {
        int16_t absVal = std::abs(resultSamples[i]);
        sumAbs += absVal;
        if (absVal > maxAbs) maxAbs = absVal;
    }

    double avgAbs = static_cast<double>(sumAbs) / resultSamples.size();

    std::cout << std::endl;
    std::cout << "Result statistics:" << std::endl;
    std::cout << "  Average absolute value: " << avgAbs << std::endl;
    std::cout << "  Peak absolute value: " << maxAbs << std::endl;
    std::cout << "  Peak dB: " << (20.0 * std::log10(maxAbs / 32768.0)) << " dB" << std::endl;

    std::cout << std::endl;

    // Write result
    WAVHeader resultHeader = fullHeader;
    resultHeader.dataSize = resultSamples.size() * sizeof(int16_t);
    resultHeader.fileSize = sizeof(WAVHeader) - 8 + resultHeader.dataSize;

    if (!WriteWAV(outputFile, resultHeader, resultSamples)) {
        return 1;
    }

    std::cout << std::endl;
    std::cout << "Subtraction completed successfully!" << std::endl;

    return 0;
}
