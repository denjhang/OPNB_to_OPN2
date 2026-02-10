#!/bin/bash
# YM2610 to YM2612 v2.5 Complete Converter
# Converts YM2610 VGM to YM2612 VGM with FM channels and ADPCM as DAC

set -e

# Check arguments
if [ $# -lt 2 ]; then
    echo "Usage: $0 input.vgm output.vgm"
    echo ""
    echo "Example:"
    echo "  $0 input.vgm output_YM2612.vgm"
    exit 1
fi

INPUT_VGM="$1"
OUTPUT_VGM="$2"

# Check if input file exists
if [ ! -f "$INPUT_VGM" ]; then
    echo "Error: Input file not found: $INPUT_VGM"
    exit 1
fi

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Tool paths
VGM2WAV_ADPCM="$SCRIPT_DIR/00_source/build/vgm2wav_adpcm_only.exe"
VGM_CONVERTER="$SCRIPT_DIR/00_source/build/vgm_converter.exe"

# Check if tools exist
if [ ! -f "$VGM2WAV_ADPCM" ]; then
    echo "Error: vgm2wav_adpcm_only.exe not found"
    exit 1
fi

if [ ! -f "$VGM_CONVERTER" ]; then
    echo "Error: vgm_converter.exe not found"
    exit 1
fi

# Create temp directory
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

echo "=== YM2610 to YM2612 v2.5 Conversion ==="
echo ""
echo "Input:  $INPUT_VGM"
echo "Output: $OUTPUT_VGM"
echo ""

# Step 1: Extract ADPCM audio
echo "Step 1/2: Extracting ADPCM audio..."
ADPCM_WAV="$TEMP_DIR/adpcm.wav"
"$VGM2WAV_ADPCM" "$INPUT_VGM" "$ADPCM_WAV" >/dev/null 2>&1

if [ ! -f "$ADPCM_WAV" ]; then
    echo "Error: Failed to extract ADPCM audio"
    exit 1
fi

echo "  ADPCM extracted: $(du -h "$ADPCM_WAV" | cut -f1)"
echo ""

# Step 2: Convert FM channels and merge with ADPCM as DAC
echo "Step 2/2: Converting FM channels and merging with ADPCM (as DAC)..."
"$VGM_CONVERTER" "$INPUT_VGM" "$ADPCM_WAV" "$OUTPUT_VGM" >/dev/null 2>&1

if [ ! -f "$OUTPUT_VGM" ]; then
    echo "Error: Failed to convert and merge"
    exit 1
fi

echo ""
echo "  Complete VGM: $(du -h "$OUTPUT_VGM" | cut -f1)"
echo ""

echo "=== Conversion Complete ==="
echo ""
echo "Output file: $OUTPUT_VGM"
echo ""
echo "v2.5 Features:"
echo "  - FM mapping: YM2610 FM6 -> YM2612 FM4"
echo "  - FM volume: TL+16 on carrier operators only"
echo "  - PCM volume: 150%"
echo "  - Working FM channels: FM1, FM2, FM3, FM5 (4 channels)"
echo ""
