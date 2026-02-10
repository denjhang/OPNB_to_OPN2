#include "WAVWriter.h"
#include <iostream>
#include <cstring>

WAVWriter::WAVWriter() : sampleRate(0), channels(0), bitsPerSample(0), dataSize(0), isOpen(false) {
}

WAVWriter::~WAVWriter() {
    if (isOpen) {
        Close();
    }
}

bool WAVWriter::Open(const std::string& fname, UINT32 rate, UINT16 ch, UINT16 bits) {
    filename = fname;
    sampleRate = rate;
    channels = ch;
    bitsPerSample = bits;
    dataSize = 0;

    file.open(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to create WAV file: " << filename << std::endl;
        return false;
    }

    // Write placeholder header
    WriteHeader();
    isOpen = true;
    return true;
}

void WAVWriter::WriteSample(INT16 sample) {
    if (!isOpen) return;

    // Write as 16-bit little-endian
    UINT8 bytes[2];
    bytes[0] = sample & 0xFF;
    bytes[1] = (sample >> 8) & 0xFF;
    file.write((char*)bytes, 2);
    dataSize += 2;
}

void WAVWriter::WriteSamples(const INT16* samples, UINT32 count) {
    for (UINT32 i = 0; i < count; i++) {
        WriteSample(samples[i]);
    }
}

bool WAVWriter::Close() {
    if (!isOpen) return false;

    // Update header with final size
    UpdateHeader();
    file.close();
    isOpen = false;

    std::cout << "WAV file saved: " << filename << std::endl;
    std::cout << "  Sample rate: " << sampleRate << " Hz" << std::endl;
    std::cout << "  Channels: " << channels << std::endl;
    std::cout << "  Bits per sample: " << bitsPerSample << std::endl;
    std::cout << "  Data size: " << dataSize << " bytes" << std::endl;
    std::cout << "  Duration: " << (dataSize / (channels * (bitsPerSample / 8)) / (float)sampleRate) << " seconds" << std::endl;

    return true;
}

void WAVWriter::WriteHeader() {
    // RIFF header
    file.write("RIFF", 4);
    UINT32 fileSize = 36 + dataSize; // Will be updated later
    file.write((char*)&fileSize, 4);
    file.write("WAVE", 4);

    // fmt chunk
    file.write("fmt ", 4);
    UINT32 fmtSize = 16;
    file.write((char*)&fmtSize, 4);
    UINT16 audioFormat = 1; // PCM
    file.write((char*)&audioFormat, 2);
    file.write((char*)&channels, 2);
    file.write((char*)&sampleRate, 4);
    UINT32 byteRate = sampleRate * channels * (bitsPerSample / 8);
    file.write((char*)&byteRate, 4);
    UINT16 blockAlign = channels * (bitsPerSample / 8);
    file.write((char*)&blockAlign, 2);
    file.write((char*)&bitsPerSample, 2);

    // data chunk
    file.write("data", 4);
    file.write((char*)&dataSize, 4); // Will be updated later
}

void WAVWriter::UpdateHeader() {
    // Update file size
    file.seekp(4);
    UINT32 fileSize = 36 + dataSize;
    file.write((char*)&fileSize, 4);

    // Update data size
    file.seekp(40);
    file.write((char*)&dataSize, 4);
}
