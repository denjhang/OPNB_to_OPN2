#include "archive_utils.h"
#include <zlib.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/types.h>
#endif

// Decompress VGZ (gzipped VGM) to VGM
bool DecompressVGZ(const std::string& vgzFile, const std::string& vgmFile) {
    gzFile gz = gzopen(vgzFile.c_str(), "rb");
    if (!gz) {
        std::cerr << "Failed to open VGZ file: " << vgzFile << std::endl;
        return false;
    }

    std::ofstream out(vgmFile, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Failed to create VGM file: " << vgmFile << std::endl;
        gzclose(gz);
        return false;
    }

    char buffer[8192];
    int bytesRead;
    while ((bytesRead = gzread(gz, buffer, sizeof(buffer))) > 0) {
        out.write(buffer, bytesRead);
    }

    gzclose(gz);
    out.close();

    return bytesRead == 0;  // 0 means EOF, -1 means error
}

// Compress VGM to VGZ
bool CompressVGZ(const std::string& vgmFile, const std::string& vgzFile) {
    std::ifstream in(vgmFile, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Failed to open VGM file: " << vgmFile << std::endl;
        return false;
    }

    gzFile gz = gzopen(vgzFile.c_str(), "wb9");  // Maximum compression
    if (!gz) {
        std::cerr << "Failed to create VGZ file: " << vgzFile << std::endl;
        return false;
    }

    char buffer[8192];
    while (in.read(buffer, sizeof(buffer)) || in.gcount() > 0) {
        if (gzwrite(gz, buffer, in.gcount()) == 0) {
            std::cerr << "Failed to write to VGZ file" << std::endl;
            gzclose(gz);
            return false;
        }
    }

    gzclose(gz);
    in.close();

    return true;
}

// Simple ZIP extraction (using system unzip command for now)
bool ExtractZip(const std::string& zipFile, const std::string& outputDir) {
    // Create output directory
    mkdir(outputDir.c_str(), 0755);

    // Use system unzip command
    std::string cmd = "unzip -q -o \"" + zipFile + "\" -d \"" + outputDir + "\"";
    int result = system(cmd.c_str());

    return result == 0;
}

// Simple ZIP creation (using system zip command for now)
bool CreateZip(const std::string& zipFile, const std::string& inputDir) {
    std::string cmd = "cd \"" + inputDir + "\" && zip -q -r \"" + zipFile + "\" .";
    int result = system(cmd.c_str());

    return result == 0;
}

// List files in ZIP (using system unzip command)
std::vector<std::string> ListZipFiles(const std::string& zipFile) {
    std::vector<std::string> files;

    std::string cmd = "unzip -l \"" + zipFile + "\"";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        return files;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        // Parse unzip -l output
        // Skip header and footer lines
        std::string line(buffer);
        if (line.find(".vgm") != std::string::npos ||
            line.find(".vgz") != std::string::npos) {
            // Extract filename from line
            size_t pos = line.rfind(' ');
            if (pos != std::string::npos) {
                std::string filename = line.substr(pos + 1);
                // Remove newline
                if (!filename.empty() && filename.back() == '\n') {
                    filename.pop_back();
                }
                files.push_back(filename);
            }
        }
    }

    pclose(pipe);
    return files;
}
