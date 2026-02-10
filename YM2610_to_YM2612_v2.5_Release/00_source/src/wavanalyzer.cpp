#include "WAVWriter.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

class WAVAnalyzer {
public:
    WAVAnalyzer() : sampleCount(0), peakValue(0), rmsSum(0.0) {
    }

    bool Analyze(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open WAV file: " << filename << std::endl;
            return false;
        }

        // Read WAV header
        char riff[4];
        file.read(riff, 4);
        if (std::string(riff, 4) != "RIFF") {
            std::cerr << "Not a valid WAV file" << std::endl;
            return false;
        }

        UINT32 fileSize;
        file.read((char*)&fileSize, 4);

        char wave[4];
        file.read(wave, 4);
        if (std::string(wave, 4) != "WAVE") {
            std::cerr << "Not a valid WAV file" << std::endl;
            return false;
        }

        // Find fmt chunk
        char fmt[4];
        file.read(fmt, 4);
        if (std::string(fmt, 4) != "fmt ") {
            std::cerr << "fmt chunk not found" << std::endl;
            return false;
        }

        UINT32 fmtSize;
        file.read((char*)&fmtSize, 4);

        UINT16 audioFormat;
        file.read((char*)&audioFormat, 2);

        UINT16 channels;
        file.read((char*)&channels, 2);

        UINT32 sampleRate;
        file.read((char*)&sampleRate, 4);

        UINT32 byteRate;
        file.read((char*)&byteRate, 4);

        UINT16 blockAlign;
        file.read((char*)&blockAlign, 2);

        UINT16 bitsPerSample;
        file.read((char*)&bitsPerSample, 2);

        // Skip to data chunk
        char data[4];
        file.read(data, 4);
        if (std::string(data, 4) != "data") {
            std::cerr << "data chunk not found" << std::endl;
            return false;
        }

        UINT32 dataSize;
        file.read((char*)&dataSize, 4);

        std::cout << "=== WAV File Analysis ===" << std::endl;
        std::cout << std::endl;
        std::cout << "File: " << filename << std::endl;
        std::cout << "Sample rate: " << sampleRate << " Hz" << std::endl;
        std::cout << "Channels: " << channels << std::endl;
        std::cout << "Bits per sample: " << bitsPerSample << std::endl;
        std::cout << "Data size: " << dataSize << " bytes" << std::endl;
        std::cout << std::endl;

        // Analyze samples
        sampleCount = dataSize / (bitsPerSample / 8);

        INT32 minValue = 0;
        INT32 maxValue = 0;
        INT64 sumAbsolute = 0;

        for (UINT32 i = 0; i < sampleCount; i++) {
            INT16 sample;
            file.read((char*)&sample, 2);

            // Track peak
            if (std::abs(sample) > peakValue) {
                peakValue = std::abs(sample);
            }

            // Track min/max
            if (sample < minValue) minValue = sample;
            if (sample > maxValue) maxValue = sample;

            // RMS calculation
            rmsSum += (double)sample * (double)sample;

            // Absolute sum for average
            sumAbsolute += std::abs(sample);
        }

        // Calculate statistics
        double rms = std::sqrt(rmsSum / sampleCount);
        double averageAbsolute = (double)sumAbsolute / sampleCount;

        // Convert to dB
        double peakDB = 20.0 * std::log10(peakValue / 32768.0);
        double rmsDB = 20.0 * std::log10(rms / 32768.0);
        double avgDB = 20.0 * std::log10(averageAbsolute / 32768.0);

        std::cout << "=== Volume Analysis ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Sample count: " << sampleCount << std::endl;
        std::cout << "Duration: " << (double)sampleCount / sampleRate << " seconds" << std::endl;
        std::cout << std::endl;

        std::cout << "Peak value: " << peakValue << " / 32768 (" << (peakValue * 100.0 / 32768.0) << "%)" << std::endl;
        std::cout << "Peak dB: " << peakDB << " dBFS" << std::endl;
        std::cout << std::endl;

        std::cout << "RMS value: " << (INT32)rms << std::endl;
        std::cout << "RMS dB: " << rmsDB << " dBFS" << std::endl;
        std::cout << std::endl;

        std::cout << "Average absolute: " << (INT32)averageAbsolute << std::endl;
        std::cout << "Average dB: " << avgDB << " dBFS" << std::endl;
        std::cout << std::endl;

        std::cout << "Min value: " << minValue << std::endl;
        std::cout << "Max value: " << maxValue << std::endl;
        std::cout << std::endl;

        // Check for issues
        std::cout << "=== Quality Check ===" << std::endl;
        std::cout << std::endl;

        if (peakValue >= 32767) {
            std::cout << "⚠ WARNING: Clipping detected! Peak value at maximum." << std::endl;
        } else if (peakValue >= 30000) {
            std::cout << "⚠ WARNING: Near clipping. Peak value very high." << std::endl;
        } else {
            std::cout << "✓ No clipping detected." << std::endl;
        }

        if (peakValue < 1000) {
            std::cout << "⚠ WARNING: Signal very quiet. Peak value < 1000." << std::endl;
        } else if (peakValue < 5000) {
            std::cout << "⚠ Signal is quiet. Consider increasing volume." << std::endl;
        } else {
            std::cout << "✓ Signal level looks good." << std::endl;
        }

        if (rms < 100) {
            std::cout << "⚠ WARNING: RMS very low. Audio may be barely audible." << std::endl;
        } else {
            std::cout << "✓ RMS level acceptable." << std::endl;
        }

        std::cout << std::endl;

        return true;
    }

private:
    UINT32 sampleCount;
    INT32 peakValue;
    double rmsSum;
};

int main(int argc, char* argv[]) {
    std::string inputFile = "Illusion_ADPCM.wav";

    if (argc >= 2) {
        inputFile = argv[1];
    }

    WAVAnalyzer analyzer;
    if (!analyzer.Analyze(inputFile)) {
        std::cerr << "Analysis failed!" << std::endl;
        return 1;
    }

    return 0;
}
