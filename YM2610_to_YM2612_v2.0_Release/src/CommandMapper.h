#ifndef COMMANDMAPPER_H
#define COMMANDMAPPER_H

#include "../libvgm/stdtype.h"
#include "VGMWriter.h"

class CommandMapper {
public:
    CommandMapper(VGMWriter& writer);
    ~CommandMapper();

    // Process YM2610 commands and convert to YM2612
    void ProcessYM2610Port0(UINT8 reg, UINT8 data);
    void ProcessYM2610Port1(UINT8 reg, UINT8 data);

    // Get statistics
    UINT32 GetFMCommandCount() const { return fmCommandCount; }
    UINT32 GetSSGCommandCount() const { return ssgCommandCount; }
    UINT32 GetADPCMCommandCount() const { return adpcmCommandCount; }

private:
    VGMWriter& writer;
    UINT32 fmCommandCount;
    UINT32 ssgCommandCount;
    UINT32 adpcmCommandCount;

    bool IsFMRegister(UINT8 reg);
    bool IsSSGRegister(UINT8 reg);
    bool IsADPCMRegister(UINT8 reg);
};

#endif // COMMANDMAPPER_H
