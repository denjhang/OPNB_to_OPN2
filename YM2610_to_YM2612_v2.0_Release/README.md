# YM2610 to YM2612 Converter v2.0

## 🎉 正式发布版本

将 YM2610 (Neo Geo) VGM 文件转换为 YM2612 (Sega Genesis/Mega Drive) VGM 文件的完整工具。

### ✨ 主要特性

- ✅ **完整的 FM 通道转换** - 4个FM通道完美映射
- ✅ **完整的 ADPCM 支持** - 使用 libvgm 渲染，100% 准确
- ✅ **硬件兼容优化** - 22.05kHz 采样率，减少 PCM 写入
- ✅ **自动采样率转换** - 22.05kHz → 44.1kHz 无缝转换
- ✅ **音量平衡优化** - FM 1/6, PCM 150%
- ✅ **VGM 格式合规** - 通过完整验证
- ✅ **批量转换支持** - 一次转换整个游戏包

## 📊 版本历史

| 版本 | 日期 | 主要改进 |
|------|------|----------|
| v1.0 | 2026-02-03 | 初始版本，手动ADPCM解码器 |
| v1.1 | 2026-02-04 | 降低采样率至22.05kHz，提升PCM音量至150% |
| v1.2 | 2026-02-04 | 修复播放速度问题，实现采样率转换 |
| v1.3 | 2026-02-04 | 优化FM音量至1/6，更好的音量平衡 |
| **v2.0** | **2026-02-04** | **正式发布，删除手动解码器，完全使用libvgm** |

## 🎯 v2.0 新特性

### 核心改进
1. **删除手动ADPCM解码器** - 完全使用libvgm渲染，确保100%准确
2. **优化硬件兼容性** - 22.05kHz采样率，减少50% PCM命令
3. **自动采样率转换** - 智能处理WAV到VGM的采样率差异
4. **音量平衡优化** - FM 1/6, PCM 150%，完美混音

### 技术规格
```
输入: YM2610 VGM/VGZ (Neo Geo)
输出: YM2612 VGM (Sega Genesis/Mega Drive)

ADPCM处理:
  - 提取: libvgm渲染 (静音FM)
  - 采样率: 22.05kHz
  - 音量: 150%
  - 编码: YM2612 DAC (8-bit unsigned)

FM处理:
  - 通道: 4个 (1:1映射)
  - 音量: 1/6 (TL +16)
  - 时钟: 8MHz (保持不变)

输出:
  - 采样率: 44.1kHz (VGM标准)
  - DAC立体声: 启用 (0xB6 = 0xC0)
  - 格式: VGM 1.51
```

## 🚀 快速开始

### 单文件转换

```bash
# 步骤 1: 提取 ADPCM (22.05kHz, 150% 音量)
bin\vgm2wav_adpcm_only.exe input.vgm adpcm.wav

# 步骤 2: 转换为 YM2612
bin\vgm_converter.exe input.vgm adpcm.wav output.vgm
```

### 批量转换

```bash
# 使用提供的批量转换脚本
./convert_batch_fixed.sh
```

## 📦 发布包内容

```
YM2610_to_YM2612_v2.0_Release/
├── bin/                          # 可执行文件
│   ├── vgm_converter.exe         # 主转换器
│   ├── vgm2wav_adpcm_only.exe    # ADPCM提取器 (libvgm)
│   ├── vgm_converter_fm_only.exe # FM转换器
│   └── ...                       # 其他工具
├── src/                          # 完整源代码
│   ├── main.cpp                  # 主程序
│   ├── CommandMapper.cpp         # FM通道映射
│   ├── VGMReader.cpp             # VGM读取
│   ├── VGMWriter.cpp             # VGM写入
│   └── ...
├── libvgm/                       # libvgm依赖库
│   ├── emu/                      # 音频模拟器
│   ├── player/                   # VGM播放器
│   └── utils/                    # 工具函数
├── examples/                     # 示例文件
│   ├── input/                    # 输入示例
│   │   └── Illusion.vgm          # 原始YM2610 VGM
│   └── output/                   # 输出示例
│       └── Illusion_YM2612.vgm   # 转换后的YM2612 VGM
├── docs/                         # 文档
│   ├── README_CN.md              # 中文文档
│   ├── RELEASE_NOTES_v2.0.md     # 发布说明
│   └── TECHNICAL.md              # 技术文档
├── scripts/                      # 批量转换脚本
│   └── convert_batch_fixed.sh
├── CMakeLists.txt                # 构建配置
└── README.md                     # 本文件
```

## 🔧 编译说明

### 前置要求
- CMake 3.12+
- C++11 编译器 (GCC/MSVC/MinGW)
- C99 编译器

### 编译步骤

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 生成的工具
- `vgm_converter.exe` - 主转换器 (FM + ADPCM)
- `vgm2wav_adpcm_only.exe` - ADPCM提取器
- `vgm_converter_fm_only.exe` - FM转换器
- `ym2610player.exe` - YM2610播放器

## 📊 性能数据

### 文件大小对比 (Illusion.vgm)
```
原始 YM2610:     260 KB
转换后 YM2612:   7.5 MB (29x)
播放时长:        44.3 秒
```

### 批量转换测试
```
Aero Fighters 3:  27 文件 → 225 MB
KOF 97:           41 文件 → 411 MB
KOF 2003:         43 文件 → 445 MB
总计:            111 文件 → 1.1 GB
```

## 🎵 音质说明

- **22.05kHz 采样率**: 对于ADPCM（鼓声、音效）完全足够
- **FM 通道**: 不受影响，保持原始质量
- **整体效果**: 硬件兼容性优秀，文件大小合理

## ⚙️ 高级选项

### 自定义采样率

```bash
# 使用 44.1kHz (更高质量，更大文件)
vgm2wav_adpcm_only.exe --samplerate=44100 input.vgm adpcm.wav

# 使用 11.025kHz (更小文件，较低质量)
vgm2wav_adpcm_only.exe --samplerate=11025 input.vgm adpcm.wav
```

## 🐛 已知问题

1. **特殊字符文件名**: 包含特殊字符（如ß）的文件可能需要手动转换
2. **VGZ格式**: 需要先解压为VGM格式

## 📝 技术细节

### YM2612 DAC 设置
```
0x52 0x2B 0x80    ; 启用 DAC
0x53 0xB6 0xC0    ; 设置通道 6 立体声输出
0x52 0x2A 0xXX    ; 写入 DAC 样本
0x70              ; 等待 1 样本
```

### 采样率转换算法
```cpp
// VGM: 44100 Hz, WAV: 22050 Hz, Ratio: 2.0
dacAccumulator += 1.0 / ratio;  // 每个VGM样本增加0.5
targetIndex = (UINT32)dacAccumulator;  // 每2个VGM样本使用1个WAV样本
```

## 🙏 致谢

- **libvgm** - 提供准确的YM2610模拟和ADPCM渲染
- **VGM规范** - 标准化的游戏音乐格式
- **Neo Geo社区** - 提供测试文件和反馈

## 📄 许可证

本项目遵循 MIT 许可证。详见 LICENSE 文件。

## 🔗 相关链接

- libvgm: https://github.com/ValleyBell/libvgm
- VGM格式规范: https://vgmrips.net/wiki/VGM_Specification

---

**版本**: 2.0
**发布日期**: 2026-02-04
**状态**: 稳定版
**作者**: YM2610 to YM2612 Converter Team
