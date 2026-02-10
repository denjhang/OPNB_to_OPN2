#ifndef VGMREADER_H
#define VGMREADER_H

#include "../libvgm/stdtype.h"
#include <vector>
#include <string>

#pragma pack(push, 1)
struct VGMHeader {
    UINT32 ident;           // 0x00: "Vgm " (0x56676D20)
    UINT32 eofOffset;       // 0x04: EOF offset (relative to 0x04)
    UINT32 version;         // 0x08: Version
    UINT32 sn76489Clock;    // 0x0C: SN76489 clock
    UINT32 ym2413Clock;     // 0x10: YM2413 clock
    UINT32 gd3Offset;       // 0x14: GD3 offset (relative to 0x14)
    UINT32 totalSamples;    // 0x18: Total samples
    UINT32 loopOffset;      // 0x1C: Loop offset (relative to 0x1C)
    UINT32 loopSamples;     // 0x20: Loop samples
    UINT32 rate;            // 0x24: Rate
    UINT16 sn76489Feedback; // 0x28: SN76489 feedback
    UINT8 sn76489ShiftWidth;// 0x2A: SN76489 shift width
    UINT8 sn76489Flags;     // 0x2B: SN76489 flags
    UINT32 ym2612Clock;     // 0x2C: YM2612 clock
    UINT32 ym2151Clock;     // 0x30: YM2151 clock
    UINT32 dataOffset;      // 0x34: VGM data offset (relative to 0x34)
    UINT32 segaPcmClock;    // 0x38: Sega PCM clock
    UINT32 spcmInterface;   // 0x3C: SPCM Interface
    UINT32 rf5c68Clock;     // 0x40: RF5C68 clock
    UINT32 ym2203Clock;     // 0x44: YM2203 clock
    UINT32 ym2608Clock;     // 0x48: YM2608 clock
    UINT32 ym2610Clock;     // 0x4C: YM2610 clock (SOURCE)
    UINT32 ym3812Clock;     // 0x50: YM3812 clock
    UINT32 ym3526Clock;     // 0x54: YM3526 clock
    UINT32 y8950Clock;      // 0x58: Y8950 clock
    UINT32 ymf262Clock;     // 0x5C: YMF262 clock
    UINT32 ymf278bClock;    // 0x60: YMF278B clock
    UINT32 ymf271Clock;     // 0x64: YMF271 clock
    UINT32 ymz280bClock;    // 0x68: YMZ280B clock
    UINT32 rf5c164Clock;    // 0x6C: RF5C164 clock
    UINT32 pwmClock;        // 0x70: PWM clock
    UINT32 ay8910Clock;     // 0x74: AY8910 clock
    UINT8 ay8910Type;       // 0x78: AY8910 chip type
    UINT8 ay8910Flags;      // 0x79: AY8910 flags
    UINT8 ym2203Flags;      // 0x7A: YM2203 flags
    UINT8 ym2608Flags;      // 0x7B: YM2608 flags
    UINT8 volumeModifier;   // 0x7C: Volume modifier
    UINT8 reserved1;        // 0x7D: Reserved
    UINT8 loopBase;         // 0x7E: Loop base
    UINT8 loopModifier;     // 0x7F: Loop modifier
};
#pragma pack(pop)

class VGMReader {
public:
    VGMReader();
    ~VGMReader();

    bool Load(const std::string& filename);
    bool IsValid() const { return valid; }

    const VGMHeader& GetHeader() const { return header; }
    const std::vector<UINT8>& GetData() const { return data; }
    UINT32 GetDataStart() const { return dataStart; }
    std::vector<UINT8> GetGD3Data() const;  // Get GD3 tag data

    // Helper functions
    static UINT32 ReadLE32(const UINT8* data);
    static UINT16 ReadLE16(const UINT8* data);

private:
    bool valid;
    VGMHeader header;
    std::vector<UINT8> data;
    UINT32 dataStart;
};

#endif // VGMREADER_H
