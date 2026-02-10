#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libvgm/player/playerbase.hpp"
#include "../libvgm/player/vgmplayer.hpp"
#include "../libvgm/player/playera.hpp"
#include "../libvgm/utils/DataLoader.h"
#include "../libvgm/utils/FileLoader.h"
#include "../libvgm/emu/SoundDevs.h"
#include "../libvgm/emu/EmuCores.h"
#include "../libvgm/emu/SoundEmu.h"
#include "WAVWriter.h"

#include <iostream>

#define BUFFER_LEN 2048

class YM2610Player {
public:
    YM2610Player() : sampleRate(44100), loops(1) {
    }

    bool RenderToWAV(const char* vgmFile, const char* wavFile) {
        PlayerA player;
        DATA_LOADER* loader;
        UINT8* buffer;
        WAVWriter wav;

        std::cout << "=== YM2610 VGM Player ===" << std::endl;
        std::cout << std::endl;

        // Register VGM player
        player.RegisterPlayerEngine(new VGMPlayer);

        // Setup output
        if (player.SetOutputSettings(sampleRate, 2, 16, BUFFER_LEN)) {
            std::cerr << "Failed to set output settings" << std::endl;
            return false;
        }

        // Configure player
        PlayerA::Config pCfg = player.GetConfiguration();
        pCfg.masterVol = 0x10000;  // 100%
        pCfg.loopCount = loops;
        pCfg.fadeSmpls = 0;  // No fade
        pCfg.endSilenceSmpls = 0;
        pCfg.pbSpeed = 1.0;
        player.SetConfiguration(pCfg);

        // Load VGM file
        loader = FileLoader_Init(vgmFile);
        if (loader == NULL) {
            std::cerr << "Failed to load VGM file: " << vgmFile << std::endl;
            return false;
        }

        UINT8 retVal = player.LoadFile(loader);
        DataLoader_Deinit(loader);

        if (retVal) {
            std::cerr << "Failed to load VGM data" << std::endl;
            return false;
        }

        std::cout << "VGM file loaded successfully" << std::endl;
        std::cout << std::endl;

        // Open WAV file
        if (!wav.Open(wavFile, sampleRate, 2, 16)) {
            return false;
        }

        // Allocate buffer
        buffer = (UINT8*)malloc(BUFFER_LEN * 2 * sizeof(INT16));
        if (buffer == NULL) {
            std::cerr << "Out of memory" << std::endl;
            return false;
        }

        // Start playback
        retVal = player.Start();
        if (retVal) {
            std::cerr << "Failed to start playback" << std::endl;
            free(buffer);
            return false;
        }

        std::cout << "Rendering audio..." << std::endl;

        UINT32 totalSamples = 0;
        UINT32 renderSamples;

        // Render loop
        while(player.GetState() < 0x02) {  // While not stopped
            renderSamples = player.Render(BUFFER_LEN, (INT16*)buffer);

            if (renderSamples == 0)
                break;

            // Write to WAV (interleaved stereo INT16)
            for (UINT32 i = 0; i < renderSamples * 2; i++) {
                wav.WriteSample(((INT16*)buffer)[i]);
            }

            totalSamples += renderSamples;

            // Progress
            if (totalSamples % (sampleRate * 5) == 0) {
                std::cout << "  " << (totalSamples / sampleRate) << " seconds rendered..." << std::endl;
            }
        }

        player.Stop();
        player.UnloadFile();

        free(buffer);
        wav.Close();

        std::cout << std::endl;
        std::cout << "Total samples rendered: " << totalSamples << std::endl;
        std::cout << "Duration: " << ((double)totalSamples / sampleRate) << " seconds" << std::endl;

        return true;
    }

    void SetSampleRate(UINT32 rate) { sampleRate = rate; }
    void SetLoops(UINT32 count) { loops = count; }

private:
    UINT32 sampleRate;
    UINT32 loops;
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input.vgm> <output.wav> [samplerate] [loops]" << std::endl;
        std::cout << "Example: " << argv[0] << " Illusion.vgm output.wav 44100 1" << std::endl;
        return 1;
    }

    YM2610Player player;

    if (argc >= 4) {
        player.SetSampleRate(atoi(argv[3]));
    }

    if (argc >= 5) {
        player.SetLoops(atoi(argv[4]));
    }

    if (!player.RenderToWAV(argv[1], argv[2])) {
        std::cerr << "Rendering failed!" << std::endl;
        return 1;
    }

    std::cout << "Success!" << std::endl;
    return 0;
}
