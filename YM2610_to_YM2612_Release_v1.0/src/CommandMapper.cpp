#include "CommandMapper.h"
#include <iostream>

CommandMapper::CommandMapper(VGMWriter& w)
    : writer(w), fmCommandCount(0), ssgCommandCount(0), adpcmCommandCount(0) {
}

CommandMapper::~CommandMapper() {
}

bool CommandMapper::IsFMRegister(UINT8 reg) {
    // FM registers: 0x20-0x2F (Timer/Mode), 0x30-0xB6 (Operator/Channel)
    return (reg >= 0x20 && reg <= 0x2F) || (reg >= 0x30 && reg <= 0xB6);
}

bool CommandMapper::IsSSGRegister(UINT8 reg) {
    // SSG registers: 0x00-0x1F
    return (reg >= 0x00 && reg <= 0x1F);
}

bool CommandMapper::IsADPCMRegister(UINT8 reg) {
    // ADPCM registers in port 1: 0x00-0x1F
    return (reg >= 0x00 && reg <= 0x1F);
}

void CommandMapper::ProcessYM2610Port0(UINT8 reg, UINT8 data) {
    // Port 0 contains:
    // - 0x00-0x1F: SSG registers (discard)
    // - 0x20-0x2F: Timer and mode registers (convert)
    // - 0x30-0xB6: FM operator/channel registers for channels 0-2 (convert)

    if (IsSSGRegister(reg)) {
        // Discard SSG registers
        ssgCommandCount++;
        return;
    }

    if (IsFMRegister(reg)) {
        // Reduce FM volume by adjusting TL (Total Level) registers
        UINT8 adjustedData = data;
        if (reg >= 0x40 && reg <= 0x4F) {
            // TL register (0x40-0x4F): 0-127, higher value = lower volume
            // To reduce volume to 1/8 (~18dB), add 18 to TL
            adjustedData = (data + 18 > 127) ? 127 : data + 18;
        }

        // Convert to YM2612 port 0 write (0x52)
        writer.WriteCommand(0x52, reg, adjustedData);
        fmCommandCount++;
        return;
    }

    // Unknown register, skip
    std::cerr << "Warning: Unknown YM2610 port 0 register: 0x"
              << std::hex << (int)reg << std::dec << std::endl;
}

void CommandMapper::ProcessYM2610Port1(UINT8 reg, UINT8 data) {
    // Port 1 contains:
    // - 0x00-0x1F: ADPCM-A and ADPCM-B control registers (handle separately)
    // - 0x30-0xB6: FM operator/channel registers for channel 3 (convert)

    if (IsADPCMRegister(reg)) {
        // ADPCM registers - will be handled by ADPCMDecoder
        adpcmCommandCount++;
        return;
    }

    if (IsFMRegister(reg)) {
        // Reduce FM volume by adjusting TL (Total Level) registers
        UINT8 adjustedData = data;
        if (reg >= 0x40 && reg <= 0x4F) {
            // TL register (0x40-0x4F): 0-127, higher value = lower volume
            // To reduce volume to 1/8 (~18dB), add 18 to TL
            adjustedData = (data + 18 > 127) ? 127 : data + 18;
        }

        // Convert to YM2612 port 1 write (0x53)
        // YM2610 channel 3 -> YM2612 channel 3
        writer.WriteCommand(0x53, reg, adjustedData);
        fmCommandCount++;
        return;
    }

    // Unknown register, skip
    std::cerr << "Warning: Unknown YM2610 port 1 register: 0x"
              << std::hex << (int)reg << std::dec << std::endl;
}
