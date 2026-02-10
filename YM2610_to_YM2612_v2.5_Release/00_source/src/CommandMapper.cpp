#include "CommandMapper.h"
#include <iostream>
#include <algorithm>
#include <cstring>

CommandMapper::CommandMapper(VGMWriter& w)
    : writer(w), fmCommandCount(0), ssgCommandCount(0), adpcmCommandCount(0), fmVolumeMultiplier(1.0) {
    // Initialize all channels to algorithm 0
    std::memset(channelAlgo, 0, sizeof(channelAlgo));
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

bool CommandMapper::IsTLRegister(UINT8 reg) {
    // TL (Total Level) registers: 0x40-0x4F
    return (reg >= 0x40 && reg <= 0x4F);
}

bool CommandMapper::IsCarrierOperator(UINT8 channel, UINT8 op) {
    // Carrier operators for each algorithm (based on YM2612 emulator code)
    // op: register slot number (0-3)
    // YM2612 slot mapping: S0=0(OP1), S1=2(OP3), S2=1(OP2), S3=3(OP4)
    // Register order: 0x40=slot0(OP1), 0x44=slot1(OP3), 0x48=slot2(OP2), 0x4C=slot3(OP4)
    static const bool carrierTable[8][4] = {
        // slot0  slot1  slot2  slot3
        // (OP1)  (OP3)  (OP2)  (OP4)
        {false, false, false, true},   // ALGO 0: only OP4 (slot3)
        {false, false, false, true},   // ALGO 1: only OP4 (slot3)
        {false, false, false, true},   // ALGO 2: only OP4 (slot3)
        {false, false, false, true},   // ALGO 3: only OP4 (slot3)
        {false, false, true, true},    // ALGO 4: OP2 (slot2) + OP4 (slot3)
        {false, true, true, true},     // ALGO 5: OP3 (slot1) + OP2 (slot2) + OP4 (slot3)
        {false, true, true, true},     // ALGO 6: OP3 (slot1) + OP2 (slot2) + OP4 (slot3)
        {true, true, true, true}       // ALGO 7: all operators
    };

    if (channel >= 6 || op >= 4) return false;
    UINT8 algo = channelAlgo[channel];
    if (algo >= 8) algo = 0;
    return carrierTable[algo][op];
}

UINT8 CommandMapper::AdjustTL(UINT8 tl) {
    // TL register: 7 bits (0-127), higher value = lower volume
    // v2.5: Use v2.0's simple addition method (+16) with v2.3's carrier detection
    // To reduce volume to 1/6 (~15.6dB), add 16 to TL
    return (tl + 16 > 127) ? 127 : tl + 16;
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
        // Track algorithm changes (0xB0-0xB2 for channels 0-2)
        if (reg >= 0xB0 && reg <= 0xB2) {
            UINT8 channel = reg & 0x03;
            channelAlgo[channel] = data & 0x07;  // Algorithm is bits 0-2
        }

        // Adjust TL (Total Level) for carrier operators only (v2.5: always adjust)
        if (IsTLRegister(reg)) {
            // Extract channel and operator from register address
            // TL registers: 0x40-0x4F
            // Format: 0x4[op][ch] where op=0-3, ch=0-2
            UINT8 channel = reg & 0x03;
            if (channel != 3) {  // channel 3 is invalid in port 0
                UINT8 op = (reg >> 2) & 0x03;
                bool isCarrier = IsCarrierOperator(channel, op);
                if (isCarrier) {
                    data = AdjustTL(data);
                }
            }
        }

        // Direct mapping - no channel remapping needed
        // YM2610 and YM2612 register layouts are compatible
        writer.WriteCommand(0x52, reg, data);
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
    // - 0x30-0xB6: FM operator/channel registers for channels 3-5 (convert)

    if (IsADPCMRegister(reg)) {
        // ADPCM registers - will be handled by ADPCMDecoder
        adpcmCommandCount++;
        return;
    }

    if (IsFMRegister(reg)) {
        // Track algorithm changes (0xB0-0xB2 for channels 3-5)
        if (reg >= 0xB0 && reg <= 0xB2) {
            UINT8 channel = (reg & 0x03) + 3;  // Port 1 = channels 3-5
            channelAlgo[channel] = data & 0x07;  // Algorithm is bits 0-2
        }

        // Adjust TL (Total Level) for carrier operators only (v2.5: always adjust)
        if (IsTLRegister(reg)) {
            // Extract channel and operator from register address
            UINT8 channel = (reg & 0x03) + 3;  // Port 1 = channels 3-5
            if (channel < 6) {
                UINT8 op = (reg >> 2) & 0x03;
                if (IsCarrierOperator(channel, op)) {
                    data = AdjustTL(data);
                }
            }
        }

        // Direct mapping - no channel remapping needed
        // YM2610 and YM2612 register layouts are compatible
        writer.WriteCommand(0x53, reg, data);
        fmCommandCount++;
        return;
    }

    // Unknown register, skip
    std::cerr << "Warning: Unknown YM2610 port 1 register: 0x"
              << std::hex << (int)reg << std::dec << std::endl;
}
