#ifndef COMMANDMAPPER_H
#define COMMANDMAPPER_H

#include "stdtype.h"
#include "VGMWriter.h"

class CommandMapper {
public:
    CommandMapper(VGMWriter& writer);
    ~CommandMapper();

    // Process YM2610 commands and convert to YM2612
    void ProcessYM2610Port0(UINT8 reg, UINT8 data);
    void ProcessYM2610Port1(UINT8 reg, UINT8 data);

    // Set FM volume adjustment (multiplier, 1.0 = no adjustment, 2.0 = half volume)
    void SetFMVolumeMultiplier(double multiplier) { fmVolumeMultiplier = multiplier; }

    // Get statistics
    UINT32 GetFMCommandCount() const { return fmCommandCount; }
    UINT32 GetSSGCommandCount() const { return ssgCommandCount; }
    UINT32 GetADPCMCommandCount() const { return adpcmCommandCount; }

private:
    VGMWriter& writer;
    UINT32 fmCommandCount;
    UINT32 ssgCommandCount;
    UINT32 adpcmCommandCount;
    double fmVolumeMultiplier;  // TL multiplier (1.0 = no change, 2.0 = half volume)
    UINT8 channelAlgo[6];  // Algorithm for each channel (0-7)

    bool IsFMRegister(UINT8 reg);
    bool IsSSGRegister(UINT8 reg);
    bool IsADPCMRegister(UINT8 reg);
    bool IsTLRegister(UINT8 reg);  // Check if register is TL (0x40-0x4F)
    bool IsCarrierOperator(UINT8 channel, UINT8 op);  // Check if operator is carrier
    UINT8 AdjustTL(UINT8 tl);  // Adjust TL value for volume reduction
};

#endif // COMMANDMAPPER_H
