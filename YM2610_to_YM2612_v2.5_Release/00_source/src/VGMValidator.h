#ifndef VGMVALIDATOR_H
#define VGMVALIDATOR_H

#include "../libvgm/stdtype.h"
#include <string>
#include <vector>

class VGMValidator {
public:
    VGMValidator();
    ~VGMValidator();

    bool Validate(const std::string& filename);
    void PrintReport() const;

private:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;

        // Statistics
        UINT32 fileSize;
        UINT32 version;
        UINT32 totalSamples;
        UINT32 commandCount;
        UINT32 dataBlockCount;
        bool hasYM2612;
        UINT32 ym2612Clock;
    };

    ValidationResult result;

    bool ValidateHeader(const std::vector<UINT8>& data);
    bool ValidateCommands(const std::vector<UINT8>& data, UINT32 dataStart);
    UINT32 GetCommandLength(UINT8 cmd, const std::vector<UINT8>& data, UINT32 pos);
};

#endif // VGMVALIDATOR_H
