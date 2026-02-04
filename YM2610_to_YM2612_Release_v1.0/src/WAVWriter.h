#ifndef WAVWRITER_H
#define WAVWRITER_H

#include "../libvgm/stdtype.h"
#include <string>
#include <vector>
#include <fstream>

class WAVWriter {
public:
    WAVWriter();
    ~WAVWriter();

    bool Open(const std::string& filename, UINT32 sampleRate, UINT16 channels, UINT16 bitsPerSample);
    void WriteSample(INT16 sample);
    void WriteSamples(const INT16* samples, UINT32 count);
    bool Close();

private:
    std::ofstream file;
    std::string filename;
    UINT32 sampleRate;
    UINT16 channels;
    UINT16 bitsPerSample;
    UINT32 dataSize;
    bool isOpen;

    void WriteHeader();
    void UpdateHeader();
};

#endif // WAVWRITER_H
