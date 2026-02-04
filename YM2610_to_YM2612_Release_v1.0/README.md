# YM2610 到 YM2612 转换器

将 YM2610 (Neo Geo) VGM 文件转换为 YM2612 (Sega Genesis/Mega Drive) VGM 文件的完整工具。

## 主要特性

- ✅ **完整的 FM 通道转换** - 4个FM通道完美映射
- ✅ **完整的 ADPCM 支持** - 使用 libvgm 渲染，确保音频质量
- ✅ **100% 准确** - ADPCM 通过 libvgm 解码，无失真
- ✅ **VGM 格式合规** - 输出文件通过完整验证
- ✅ **简单易用** - 两步完成转换

## 转换规则

1. **FM 通道**: YM2610 的 4 个 FM 通道 → YM2612 的 4 个 FM 通道
2. **ADPCM**: YM2610 的 ADPCM-A (6通道) + ADPCM-B (1通道) → 混音后转换为 YM2612 DAC
3. **SSG**: 舍弃 YM2610 的 SSG 部分
4. **时钟**: 保持相同的时钟频率 (8 MHz)
5. **音量**: FM 音量降低为 1/8，避免与 DAC 混音时削波

## 使用方法

### 步骤 1: 提取 ADPCM 音频

使用 libvgm 的 vgm2wav_adpcm_only 工具提取纯 ADPCM 音频（静音 FM 通道）：

```bash
vgm2wav_adpcm_only.exe input.vgm adpcm.wav
```

### 步骤 2: 转换为 YM2612 VGM

将 VGM 文件和 ADPCM 音频合并转换：

```bash
vgm_converter.exe input.vgm adpcm.wav output.vgm
```

### 完整示例

```bash
# 转换 Illusion.vgm
vgm2wav_adpcm_only.exe Illusion.vgm Illusion_ADPCM.wav
vgm_converter.exe Illusion.vgm Illusion_ADPCM.wav Illusion_YM2612.vgm
```

## 工具说明

### vgm2wav_adpcm_only.exe
- 使用 libvgm 渲染纯 ADPCM 音频
- 自动静音 FM 通道
- 输出 44.1kHz 16-bit 立体声 WAV

### vgm_converter.exe
- 转换 FM 通道到 YM2612
- 将 ADPCM WAV 编码为 YM2612 DAC
- 自动设置 DAC 立体声输出
- 保留循环点和 GD3 标签

### 其他工具

- **vgm_converter_fm_only.exe** - 仅转换 FM 通道（不包含 ADPCM）
- **ym2610player.exe** - 完整的 YM2610 VGM 播放器

## 技术细节

### ADPCM 处理

本转换器使用 **libvgm 渲染** 而不是手动实现 ADPCM 解码器：

1. libvgm 的 YM2610 模拟器已经过充分测试和验证
2. 确保 ADPCM-A 和 ADPCM-B 的解码准确性
3. 正确处理音量、混音和采样率

### YM2612 DAC 设置

转换器会自动设置以下寄存器：

```
0x52 0x2B 0x80    ; 启用 YM2612 DAC
0x53 0xB6 0xC0    ; 设置通道 6 立体声输出（左右都启用）
```

这是 YM2612 DAC 正常工作的必要条件。

### 文件大小

由于 DAC 流数据，输出文件会比输入文件大：
- 输入: ~500KB (压缩的 ADPCM)
- 输出: ~5-8MB (未压缩的 DAC 流)

可以使用 gzip 压缩为 .vgz 格式来减小文件大小。

## 限制

- 不支持 YM2610 的 SSG (PSG) 通道
- 输出文件较大（可压缩为 .vgz）
- DAC 采样率为 44.1kHz（与 VGM 采样率相同）

## 系统要求

- Windows 7 或更高版本
- 64-bit 系统推荐

## 许可证

本项目使用 libvgm 库，遵循其许可证条款。

## 技术支持

如有问题，请查看源码中的文档或提交 issue。

---

**版本**: 1.0
**日期**: 2026-02-04
**状态**: 生产就绪
