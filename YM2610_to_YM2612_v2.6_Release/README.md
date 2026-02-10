# YM2610 to YM2612 VGM Converter v2.6

将Neo Geo (YM2610/YM2610B) VGM音乐文件转换为Sega Genesis/Mega Drive (YM2612) VGM格式。

**v2.6专门优化**: 针对Super_Dodge_Ball等需要更强FM音量降低的游戏。

## 版本特性

### v2.6核心特性

- ✅ **FM通道映射**: YM2610 FM6 → YM2612 FM4 (4个FM通道工作)
- ✅ **强化音量降低**: TL×2.5 (仅载波运算符，音量降低到40%)
- ✅ **载波检测**: 支持8种FM算法的精确载波运算符识别
- ✅ **PCM转换**: ADPCM转DAC，音量150%
- ✅ **GD3保留**: 保留元数据标签
- ✅ **硬件兼容**: 基于v2.5验证的稳定架构

### v2.6 vs v2.5对比

| 特性 | v2.5 | v2.6 |
|------|------|------|
| FM音量方法 | TL+16 | TL×2.5 |
| 音量效果 | 降低约15% | 降低到40% |
| 适用场景 | 大多数游戏 | Super_Dodge_Ball等特殊游戏 |
| 载波检测 | 是 | 是 |
| PCM音量 | 150% | 150% |

### 为什么需要v2.6？

某些Neo Geo游戏（如Super_Dodge_Ball）的FM音轨音量相对PCM过大，使用v2.5的TL+16方法降低不够。v2.6采用v2.3的TL×2.5方法，将FM音量降低到40%，获得更好的FM/PCM平衡。

## 快速开始

### 单个游戏转换

```bash
# 从ZIP文件转换
./convert_complete.sh "Super_Dodge_Ball_(Neo_Geo).zip"

# 从目录转换
./convert_complete.sh /path/to/vgm_files/
```

### 指定输出目录

```bash
./convert_complete.sh input.zip custom_output_dir
```

## 技术细节

### FM通道处理

#### 通道映射
- YM2610 FM0-2 → YM2612 FM0-2 (port 0)
- YM2610 FM3-5 → YM2612 FM3-5 (port 1)
- YM2610 FM6 → YM2612 FM4 (重映射)

实际工作通道：FM1, FM2, FM3, FM5 (4个通道)

#### 音量调整算法

```cpp
UINT8 AdjustTL(UINT8 tl) {
    tl = tl & 0x7F;  // 确保7位值
    UINT16 adjusted = (tl * 5) / 2;  // 乘以2.5
    return (adjusted > 127) ? 127 : (UINT8)adjusted;
}
```

- TL寄存器：7位 (0-127)
- TL值越大，音量越小
- TL×2.5 = 音量降低到约40%

#### 载波运算符表

```
算法 0-3: 仅OP4 (slot3)
算法 4:   OP2 (slot2) + OP4 (slot3)
算法 5-6: OP3 (slot1) + OP2 (slot2) + OP4 (slot3)
算法 7:   所有运算符
```

只调整载波运算符的TL值，保持调制器不变，最小化音色失真。

### PCM/ADPCM处理

- **提取**: 使用vgm2wav_adpcm_only.exe提取ADPCM音频
- **音量**: 150% (0x18000)
- **格式**: 转换为YM2612 DAC格式
- **采样率**: 保持原始采样率
- **通道**: 使用FM6作为DAC输出

### 转换流程

```
输入VGM (YM2610)
    ↓
[vgm2wav_adpcm_only.exe]
    ↓
ADPCM音频 (WAV)
    ↓
[vgm_converter.exe] ← 输入VGM
    ↓
输出VGM (YM2612)
```

## 文件结构

```
YM2610_to_YM2612_v2.6_Release/
├── 00_source/
│   ├── src/                    # C++源代码
│   │   ├── main.cpp           # 主程序
│   │   ├── CommandMapper.cpp  # FM命令映射 (TL×2.5)
│   │   ├── VGMReader.cpp      # VGM读取
│   │   ├── VGMWriter.cpp      # VGM写入
│   │   └── vgm2wav_adpcm_only.cpp  # ADPCM提取
│   ├── build/                 # 编译输出
│   │   ├── vgm_converter.exe  # 主转换器 (189KB)
│   │   └── vgm2wav_adpcm_only.exe  # ADPCM提取器 (2.2MB)
│   └── CMakeLists.txt         # CMake配置
├── libvgm-modizer/
│   └── libvgm/                # libvgm库 (34MB)
├── examples/
│   └── output/                # 转换示例
│       ├── 01 Title_YM2612.vgm
│       └── 02 Team Select_YM2612.vgm
├── convert_complete.sh        # 批量转换脚本
├── README.md                  # 本文件
├── BUILD.md                   # 编译说明
├── RELEASE_NOTES.md           # 版本更新说明
└── VERSION.txt                # 版本信息
```

## 系统要求

### 运行要求
- Windows with MSYS2/MinGW64
- 或任何支持bash的环境

### 编译要求
- CMake 3.12+
- GCC/G++ 编译器
- zlib库

详见 [BUILD.md](BUILD.md)

## 使用示例

### Super_Dodge_Ball转换

```bash
# 转换整个游戏
./convert_complete.sh "Super_Dodge_Ball_(Neo_Geo).zip"

# 输出到02_output/Super_Dodge_Ball_v2.6/
# 包含16个VGM文件，总大小约160MB
```

### 单文件转换

```bash
# 解压VGZ
gunzip -c "01 Title.vgz" > input.vgm

# 提取ADPCM
./00_source/build/vgm2wav_adpcm_only.exe input.vgm adpcm.wav

# 转换
./00_source/build/vgm_converter.exe input.vgm adpcm.wav output.vgm
```

## 已知限制

1. **SSG通道**: YM2610的SSG (PSG) 通道会被丢弃
2. **FM4通道**: 被FM6覆盖，实际只有4个FM通道工作
3. **文件大小**: DAC数据未压缩，文件较大
   - 44.1kHz采样率 ≈ 176.4KB/秒
   - 3分钟曲目 ≈ 32MB
4. **平台**: 需要MSYS2/MinGW64环境编译

## 版本选择指南

### 使用v2.5的情况
- 大多数Neo Geo游戏
- Money_Puzzle_Exchanger
- KOF系列
- 一般的音乐转换

### 使用v2.6的情况
- Super_Dodge_Ball
- FM音轨明显过响的游戏
- 需要更强FM音量降低的场景

## 故障排除

### 转换后没有FM声音
- 检查VGM播放器是否支持YM2612
- 尝试使用VGMPlay或in_vgm播放器
- 确认原始VGM文件包含FM数据

### 文件过大
- 这是正常的，DAC数据未压缩
- 可以使用gzip压缩为.vgz格式
- 或使用VGM优化工具

### 音量不平衡
- v2.6已针对Super_Dodge_Ball优化
- 如需调整，修改CommandMapper.cpp中的AdjustTL函数
- 或修改vgm2wav_adpcm_only.cpp中的PCM音量

## 技术支持

如遇问题，请检查：
1. [README.md](README.md) - 使用说明
2. [BUILD.md](BUILD.md) - 编译说明
3. [RELEASE_NOTES.md](RELEASE_NOTES.md) - 版本更新详情

## 许可证

本项目使用libvgm库，遵循其相应的许可证。

---

YM2610 to YM2612 VGM Converter v2.6
© 2026
专门优化：Super_Dodge_Ball及类似游戏
