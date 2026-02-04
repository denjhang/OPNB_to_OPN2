#ifndef VGMWRITER_H
#define VGMWRITER_H

#include "../libvgm/stdtype.h"
#include "VGMReader.h"
#include <vector>
#include <string>

class VGMWriter {
public:
    VGMWriter();
    ~VGMWriter();

    void Initialize(const VGMHeader& sourceHeader, UINT32 ym2612Clock);
    void WriteCommand(UINT8 cmd);
    void WriteCommand(UINT8 cmd, UINT8 data1);
    void WriteCommand(UINT8 cmd, UINT8 data1, UINT8 data2);
    void WriteCommand(UINT8 cmd, UINT16 data);
    void WriteDataBlock(UINT8 type, const std::vector<UINT8>& blockData);

    void MarkLoopPoint();  // Mark current position as loop point
    void SetGD3Data(const std::vector<UINT8>& gd3Data);  // Set GD3 tag data

    bool Save(const std::string& filename);

    // Helper functions
    static void WriteLE32(std::vector<UINT8>& data, UINT32 offset, UINT32 value);
    static void WriteLE16(std::vector<UINT8>& data, UINT32 offset, UINT16 value);

private:
    VGMHeader header;
    std::vector<UINT8> commandData;
    std::vector<UINT8> dataBlocks;
    std::vector<UINT8> gd3Data;
    UINT32 loopCommandOffset;  // Offset in commandData where loop starts
    bool hasLoopPoint;
};

#endif // VGMWRITER_H
