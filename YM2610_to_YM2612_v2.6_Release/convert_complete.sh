#!/bin/bash
# YM2610 to YM2612 v2.6 Converter
# Specialized for games requiring stronger FM volume reduction (like Super_Dodge_Ball)

set -e

# Check if input is a zip file or directory
if [ $# -lt 1 ]; then
    echo "Usage: $0 <input.zip or input_dir> [output_dir]"
    exit 1
fi

INPUT="$1"
OUTPUT_DIR="${2:-02_output}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Tool paths for v2.6
VGM_CONVERTER="$SCRIPT_DIR/00_source/build/vgm_converter.exe"
VGM2WAV_ADPCM="$SCRIPT_DIR/00_source/build/vgm2wav_adpcm_only.exe"

# Check if tools exist
if [ ! -f "$VGM_CONVERTER" ]; then
    echo "Error: vgm_converter.exe not found in 00_source/build/"
    exit 1
fi

if [ ! -f "$VGM2WAV_ADPCM" ]; then
    echo "Error: vgm2wav_adpcm_only.exe not found in 00_source/build/"
    exit 1
fi

# Create temp directory
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

# Handle zip file
if [[ "$INPUT" == *.zip ]]; then
    echo "=== Extracting ZIP file ==="
    unzip -q "$INPUT" -d "$TEMP_DIR"
    GAME_NAME=$(basename "$INPUT" .zip)
    INPUT_DIR="$TEMP_DIR"
else
    GAME_NAME=$(basename "$INPUT")
    INPUT_DIR="$INPUT"
fi

# Create output directory
FINAL_OUTPUT_DIR="$OUTPUT_DIR/${GAME_NAME}_v2.6"
mkdir -p "$FINAL_OUTPUT_DIR"

echo "=== YM2610 to YM2612 v2.6 Batch Conversion ==="
echo "Game: $GAME_NAME"
echo "Output: $FINAL_OUTPUT_DIR"
echo ""

converted=0
failed=0

# Find all VGZ and VGM files
for vgzfile in "$INPUT_DIR"/*.vgz "$INPUT_DIR"/*/*.vgz; do
    if [ ! -f "$vgzfile" ]; then
        continue
    fi

    filename=$(basename "$vgzfile" .vgz)
    echo "Converting: $filename"

    # Decompress VGZ to VGM
    vgmfile="$TEMP_DIR/${filename}.vgm"
    gunzip -c "$vgzfile" > "$vgmfile" 2>/dev/null || {
        echo "  Failed to decompress"
        ((failed++))
        continue
    }

    # Step 1: Extract ADPCM
    wavfile="$TEMP_DIR/${filename}.wav"
    "$VGM2WAV_ADPCM" "$vgmfile" "$wavfile" >/dev/null 2>&1 || {
        echo "  Failed to extract ADPCM"
        ((failed++))
        rm -f "$vgmfile"
        continue
    }

    # Step 2: Convert with DAC
    output_vgm="$FINAL_OUTPUT_DIR/${filename}_YM2612.vgm"
    "$VGM_CONVERTER" "$vgmfile" "$wavfile" "$output_vgm" >/dev/null 2>&1 || {
        echo "  Failed to convert"
        ((failed++))
        rm -f "$vgmfile" "$wavfile"
        continue
    }

    size=$(du -h "$output_vgm" | cut -f1)
    echo "  Success: $size"
    ((converted++))

    # Cleanup temp files
    rm -f "$vgmfile" "$wavfile"
done

# Also process plain VGM files
for vgmfile in "$INPUT_DIR"/*.vgm "$INPUT_DIR"/*/*.vgm; do
    if [ ! -f "$vgmfile" ]; then
        continue
    fi

    filename=$(basename "$vgmfile" .vgm)
    echo "Converting: $filename"

    # Step 1: Extract ADPCM
    wavfile="$TEMP_DIR/${filename}.wav"
    "$VGM2WAV_ADPCM" "$vgmfile" "$wavfile" >/dev/null 2>&1 || {
        echo "  Failed to extract ADPCM"
        ((failed++))
        continue
    }

    # Step 2: Convert with DAC
    output_vgm="$FINAL_OUTPUT_DIR/${filename}_YM2612.vgm"
    "$VGM_CONVERTER" "$vgmfile" "$wavfile" "$output_vgm" >/dev/null 2>&1 || {
        echo "  Failed to convert"
        ((failed++))
        rm -f "$wavfile"
        continue
    }

    size=$(du -h "$output_vgm" | cut -f1)
    echo "  Success: $size"
    ((converted++))

    # Cleanup temp files
    rm -f "$wavfile"
done

echo ""
echo "=== Conversion Complete ==="
echo "Converted: $converted files"
echo "Failed: $failed files"
echo "Output directory: $FINAL_OUTPUT_DIR"
echo ""
echo "v2.6 Features:"
echo "  - FM mapping: FM6 -> FM4 (4 FM channels working)"
echo "  - FM volume: TL×2.5 on carrier operators only (40% volume)"
echo "  - PCM volume: 150%"
echo "  - Carrier detection: Yes"
echo "  - Optimized for: Super_Dodge_Ball and similar games"
