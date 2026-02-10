# YM2610 to YM2612 VGM Converter v2.5

## 概述

YM2610 to YM2612 VGM Converter v2.5 是一个将Neo Geo (YM2610/YM2610B) VGM音乐文件转换为Sega Genesis/Mega Drive (YM2612) VGM格式的工具。

## 版本信息

- **版本**: v2.5
- **发布日期**: 2026-02-10
- **支持平台**: Windows (MSYS2/MinGW64)

## 主要特性

### v2.5 核心功能
✅ **完整转换**: FM通道 + ADPCM转DAC
✅ **智能音量调整**: 仅调整载波运算符，保持音色
✅ **TL修复**: 正确处理TL寄存器的7位限制
✅ **载波检测**: 基于FM算法的载波运算符识别
✅ **音量优化**: ADPCM 150%, FM TL+16
✅ **GD3保留**: 保留原始VGM的元数据标签
✅ **硬件兼容**: 在实际硬件上测试稳定

### 技术特性

#### FM通道处理
- **映射方式**: YM2610 FM6 → YM2612 FM4
- **工作通道**: FM1, FM2, FM3, FM5 (4个通道)
- **音量调整**: TL+16 (简单加法)
- **调整范围**: 仅载波运算符
- **算法跟踪**: 是 (0xB0-0xB2寄存器)

#### 载波运算符表
```
ALGO 0-3: 仅OP4 (slot3)
ALGO 4:   OP2 (slot2) + OP4 (slot3)
ALGO 5-6: OP3 (slot1) + OP2 (slot2) + OP4 (slot3)
ALGO 7:   所有运算符
```

#### PCM/ADPCM处理
- **音量**: 150% (0x18000)
- **格式**: 转换为YM2612 DAC
- **采样率**: 保持原始采样率
- **通道**: 使用FM6作为DAC输出

## 快速开始

### 1. 系统要求

- Windows with MSYS2/MinGW64
- CMake 3.12+
- GCC/G++ 编译器
- zlib 库

### 2. 使用方法

#### 单文件转换
```bash
./convert_complete.sh input.vgm output.vgm
```

#### 批量转换
```bash
# 转换目录中的所有VGZ文件
for vgzfile in *.vgz; do
    filename=$(basename "$vgzfile" .vgz)
    gunzip -c "$vgzfile" > "${filename}.vgm"
    ./convert_complete.sh "${filename}.vgm" "${filename}_YM2612.vgm"
    rm "${filename}.vgm"
done
```

### 3. 转换流程

```
输入VGM (YM2610)
    ↓
[步骤1] vgm2wav_adpcm_only.exe
    ↓
ADPCM音频 (WAV)
    ↓
[步骤2] vgm_converter.exe
    ↓
输出VGM (YM2612)
```

## 编译说明

### 编译步骤

```bash
cd 00_source
mkdir build
cd build

# 复制依赖库
mkdir lib
cp ../../libvgm-modizer/libvgm/build/bin/*.a lib/

# 配置和编译
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### 编译输出

- `vgm_converter.exe` - FM通道转换器 (带DAC支持)
- `vgm2wav_adpcm_only.exe` - ADPCM音频提取器

## 版本对比

| 特性 | v2.0 | v2.3 | v2.4 | v2.5 |
|------|------|------|------|------|
| FM映射 | FM6→FM1 | FM6→FM4 | FM6→FM4 | FM6→FM4 |
| 工作FM通道 | 3个 | 4个 | 4个 | 4个 |
| FM音量方法 | TL+16 | TL×2.5 | TL+16 | TL+16 |
| 调整范围 | 全部OP | 仅载波 | 全部OP | 仅载波 |
| 载波检测 | 否 | 是 | 否 | 是 |
| PCM音量 | 150% | 200% | 150% | 150% |
| 硬件兼容性 | FM缺失 | PCM干扰 | 良好 | 最佳 |

## v2.5 优势

### 1. 完整的FM通道
- 4个FM通道正常工作 (FM1, FM2, FM3, FM5)
- 使用v2.3的FM6→FM4映射，避免覆盖FM1

### 2. 保持音色
- 只调整载波运算符的TL值
- 调制器运算符保持不变
- 音色失真最小化

### 3. 硬件兼容性
- PCM音量150%，避免硬件干扰
- 使用v2.0验证过的TL+16方法
- 在实际硬件上测试稳定

### 4. 混合最佳实践
- v2.0的稳定性 + v2.3的载波检测
- 简单可靠的TL调整算法
- 经过验证的PCM音量设置

## 技术细节

### TL寄存器处理
```cpp
UINT8 CommandMapper::AdjustTL(UINT8 tl) {
    // 屏蔽第7位
    tl = tl & 0x7F;

    // 使用简单加法 (+16)
    return (tl + 16 > 127) ? 127 : tl + 16;
}
```

### 载波检测
```cpp
bool CommandMapper::IsCarrierOperator(UINT8 channel, UINT8 op) {
    static const bool carrierTable[8][4] = {
        {false, false, false, true},   // ALGO 0
        {false, false, false, true},   // ALGO 1
        {false, false, false, true},   // ALGO 2
        {false, false, false, true},   // ALGO 3
        {false, false, true, true},    // ALGO 4
        {false, true, true, true},     // ALGO 5
        {false, true, true, true},     // ALGO 6
        {true, true, true, true}       // ALGO 7
    };

    UINT8 algo = channelAlgo[channel];
    return carrierTable[algo][op];
}
```

### FM6映射
```cpp
// 在 main.cpp 中
if (reg == 0x28) {  // Key On/Off
    UINT8 channel = val & 0x07;
    if (channel == 6) {
        // 将FM6重映射到FM4
        val = (val & 0xF8) | 4;
    }
}
```

## 文件大小说明

VGM文件较大的原因：
- DAC数据未压缩
- 每个采样4字节 (0x52 0x2A [sample] 0x70)
- 44.1kHz采样率 = 176.4KB/秒
- 长曲目会产生大文件 (例如：3分钟 ≈ 32MB)

## 已知限制

1. **SSG通道**: YM2610的SSG (PSG) 通道会被丢弃
2. **FM4通道**: 被FM6覆盖，实际只有4个FM通道工作
3. **文件大小**: DAC数据导致文件较大
4. **平台**: 需要MSYS2/MinGW64环境编译

## 测试游戏

v2.5已成功转换以下游戏：

### KOF系列 (8个)
- The King of Fighters '96, '97, '98, '99
- The King of Fighters 2000, 2001, 2002, 2003

### 其他游戏 (40+个)
- Money Puzzle Exchanger
- Magical Drop II
- Super Dodge Ball
- Aero Fighters 3
- Blazing Star
- Pulstar
- Garou - Mark of the Wolves
- Shock Troopers
- Rage of the Dragons
- 等等...

**总计**: 48+个游戏，1100+个VGM文件

## 使用建议

### 播放测试
建议使用以下播放器测试：
- VGMPlay (PC)
- in_vgm (Winamp插件)
- 实际硬件 (Sega Genesis/Mega Drive)

### 音量调整
如果需要调整音量：
- **FM音量**: 修改 `CommandMapper.cpp` 中的 `AdjustTL()` 函数
- **PCM音量**: 修改 `vgm2wav_adpcm_only.cpp` 中的 `pCfg.masterVol`

### 重新编译
```bash
cd 00_source/build
cmake --build . --config Release
```

## 文件结构

```
YM2610_to_YM2612_v2.5_Release/
├── README.md                    # 本文件
├── BUILD.md                     # 编译说明
├── RELEASE_NOTES.md            # 版本更新说明
├── VERSION.txt                  # 版本信息
├── convert_complete.sh          # 转换脚本
├── 00_source/                   # 源代码
│   ├── src/                     # C++ 源文件
│   ├── CMakeLists.txt          # CMake 配置
│   └── build/                   # 编译输出
│       ├── vgm_converter.exe
│       └── vgm2wav_adpcm_only.exe
├── libvgm-modizer/             # libvgm 依赖
│   └── libvgm/
│       ├── build/bin/          # 预编译库
│       ├── player/             # 头文件
│       ├── emu/                # 头文件
│       └── utils/              # 头文件
├── examples/                    # 示例文件
│   ├── input/                  # 输入示例
│   └── output/                 # 输出示例
└── docs/                        # 文档
```

## 许可证

本项目使用 libvgm 库，遵循其相应的许可证。

## 更新日志

### v2.5 (2026-02-10)
- 结合v2.0的TL+16方法和v2.3的载波检测
- PCM音量调整为150%以提高硬件兼容性
- 保持v2.3的FM6→FM4映射
- 优化载波运算符识别算法
- 大规模测试：48+个游戏，1100+个VGM文件

### v2.4 (2026-02-09)
- 使用v2.3的FM6→FM4映射
- 使用v2.0的TL+16音量调整方法
- PCM音量150%

### v2.3 (2026-02-09)
- 实现载波运算符检测
- 只调整载波运算符的TL值
- 修复TL寄存器溢出问题
- PCM音量200%

### v2.0 (2026-02-08)
- 初始版本
- 基本FM通道转换
- ADPCM转DAC功能
- PCM音量150%

---

YM2610 to YM2612 VGM Converter v2.5
© 2026
