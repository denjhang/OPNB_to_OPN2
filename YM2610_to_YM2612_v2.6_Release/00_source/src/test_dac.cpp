#include "../src/VGMWriter.h"
#include <iostream>
#include <cmath>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main() {
    VGMWriter writer;

    // Initialize with minimal header
    VGMHeader header;
    memset(&header, 0, sizeof(header));
    header.version = 0x151;
    header.totalSamples = 44100 * 2;  // 2 seconds

    writer.Initialize(header, 8000000);  // 8 MHz YM2612

    // Enable DAC
    writer.WriteCommand(0x52, 0x2B, 0x80);

    // Set channel 6 pan to both speakers
    writer.WriteCommand(0x53, 0xB6, 0xC0);

    // Generate a 440 Hz sine wave for 2 seconds
    const double freq = 440.0;
    const double sampleRate = 44100.0;
    const int totalSamples = 44100 * 2;

    for (int i = 0; i < totalSamples; i++) {
        // Generate sine wave
        double t = i / sampleRate;
        double value = sin(2.0 * M_PI * freq * t);

        // Convert to 8-bit unsigned (0-255, centered at 128)
        UINT8 sample = static_cast<UINT8>((value * 127.0) + 128.0);

        // Write DAC sample
        writer.WriteCommand(0x52, 0x2A, sample);

        // Write wait command for 1 sample
        writer.WriteCommand(0x70);  // 0x70 = wait 1 sample
    }

    // End of data
    writer.WriteCommand(0x66);

    // Save
    if (writer.Save("test_dac_sine.vgm")) {
        std::cout << "Created test_dac_sine.vgm" << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to create test file" << std::endl;
        return 1;
    }
}
