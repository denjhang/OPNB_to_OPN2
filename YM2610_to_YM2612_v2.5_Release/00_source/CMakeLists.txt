cmake_minimum_required(VERSION 3.12)
project(YM2610_to_YM2612 VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

# Platform-specific definitions
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # Use static runtime for MSVC
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Check for stdint.h
include(CheckIncludeFile)
check_include_file("stdint.h" HAVE_STDINT)
if(HAVE_STDINT)
    add_definitions(-DHAVE_STDINT_H)
endif()

# Endianness detection
include(TestBigEndian)
test_big_endian(ARCH_BIG_ENDIAN)
if(ARCH_BIG_ENDIAN)
    add_definitions(-DVGM_BIG_ENDIAN)
else()
    add_definitions(-DVGM_LITTLE_ENDIAN)
endif()

# Source files (removed ADPCMDecoder - use libvgm for ADPCM rendering)
set(SOURCES
    src/main.cpp
    src/VGMReader.cpp
    src/VGMWriter.cpp
    src/CommandMapper.cpp
    src/VGMValidator.cpp
)

# Executable
add_executable(vgm_converter ${SOURCES})

# ADPCM to WAV converter (removed - use vgm2wav_adpcm_only with libvgm instead)
# add_executable(adpcm2wav
#     src/adpcm2wav.cpp
#     src/VGMReader.cpp
#     src/VGMWriter.cpp
#     src/WAVWriter.cpp
# )

# WAV analyzer
add_executable(wavanalyzer
    src/wavanalyzer.cpp
)

# FM-only converter
add_executable(vgm_converter_fm_only
    src/main_fm_only.cpp
    src/VGMReader.cpp
    src/VGMWriter.cpp
    src/CommandMapper.cpp
    src/VGMValidator.cpp
)

# WAV subtraction tool
add_executable(wav_subtract
    src/wav_subtract.cpp
)

# VGM converter with DAC
add_executable(vgm_converter_with_dac
    src/main_with_dac.cpp
    src/VGMReader.cpp
    src/VGMWriter.cpp
    src/CommandMapper.cpp
    src/VGMValidator.cpp
)

# Include directories
target_include_directories(vgm_converter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libvgm
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# target_include_directories(adpcm2wav PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/libvgm
#     ${CMAKE_CURRENT_SOURCE_DIR}/src
# )

target_include_directories(wavanalyzer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libvgm
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(vgm_converter_fm_only PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libvgm
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(wav_subtract PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(vgm_converter_with_dac PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libvgm
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Installation (removed adpcm2wav - use vgm2wav_adpcm_only instead)
install(TARGETS vgm_converter wavanalyzer vgm_converter_fm_only wav_subtract vgm_converter_with_dac DESTINATION bin)

# Include YM2610 player
include(CMakeLists_ym2610player.txt)
install(TARGETS ym2610player DESTINATION bin)

# Include vgm2wav_adpcm_only
include(CMakeLists_vgm2wav.txt)
install(TARGETS vgm2wav_adpcm_only DESTINATION bin)

# Print configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
