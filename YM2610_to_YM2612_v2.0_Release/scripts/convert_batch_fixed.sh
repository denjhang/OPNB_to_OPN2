#!/bin/bash
# Batch convert with proper file handling

convert_dir() {
    local src_dir="$1"
    local dst_dir="${src_dir}_YM2612"
    
    mkdir -p "$dst_dir"
    
    local total=$(find "$src_dir" -name "*.vgm" | wc -l)
    local count=0
    
    echo "=== Converting $src_dir ==="
    echo "Total files: $total"
    echo ""
    
    find "$src_dir" -name "*.vgm" | while IFS= read -r vgm; do
        count=$((count + 1))
        local name=$(basename "$vgm" .vgm)
        
        printf "[%3d/%3d] %s\n" $count $total "$name"
        
        # Extract ADPCM
        ./build/vgm2wav_adpcm_only.exe "$vgm" "$dst_dir/${name}_adpcm.wav" > /dev/null 2>&1
        
        # Convert
        ./build/vgm_converter.exe "$vgm" "$dst_dir/${name}_adpcm.wav" "$dst_dir/${name}.vgm" > /dev/null 2>&1
        
        # Cleanup temp WAV
        rm -f "$dst_dir/${name}_adpcm.wav"
    done
    
    echo "✓ Completed: $dst_dir"
    echo ""
}

# Convert all three games
convert_dir "Aero_Fighters_3_(Neo_Geo)"
convert_dir "kof97_vgm"
convert_dir "kof2003_vgm"

echo "=== All conversions completed! ==="
