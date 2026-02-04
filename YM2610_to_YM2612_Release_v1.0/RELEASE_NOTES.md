# YM2610 到 YM2612 转换器 v1.0 - 发布说明

## 发布信息

- **版本**: 1.0
- **日期**: 2026-02-04
- **状态**: 生产就绪
- **平台**: Windows (64-bit)

## 发布内容

### 可执行文件 (bin/)

1. **vgm2wav_adpcm_only.exe** (2.2 MB)
   - 使用 libvgm 提取纯 ADPCM 音频
   - 自动静音 FM 通道
   - 输出 44.1kHz 16-bit 立体声 WAV

2. **vgm_converter.exe** (349 KB)
   - 主转换器：FM + ADPCM as DAC
   - 自动设置 DAC 立体声输出
   - 保留循环点和 GD3 标签

3. **vgm_converter_fm_only.exe** (309 KB)
   - 仅转换 FM 通道
   - 不包含 ADPCM

4. **batch_converter.exe** (727 KB)
   - 批量转换工具
   - 支持 ZIP/VGZ 格式
   - 自动处理整个文件夹

5. **其他工具**
   - wav_subtract.exe - WAV 音频减法
   - wavanalyzer.exe - WAV 分析工具
   - test_dac.exe - DAC 测试工具

### 源代码 (src/)

- 完整的 C++ 源代码
- CMake 构建配置
- 包含所有必要的头文件

### 依赖库 (libvgm/)

- libvgm 核心库源码
- YM2610/YM2612 模拟器
- VGM 播放器和工具

### 文档 (docs/)

- README_CN.md - 完整中文文档
- 项目完成报告_FINAL.md - 技术细节

## 使用方法

### 快速开始

```bash
# 1. 提取 ADPCM 音频
bin\vgm2wav_adpcm_only.exe input.vgm adpcm.wav

# 2. 转换为 YM2612 VGM
bin\vgm_converter.exe input.vgm adpcm.wav output.vgm
```

### 批量转换

```bash
# 转换整个 ZIP 包
bin\batch_converter.exe input.zip output_folder
```

## 技术特性

### 音频质量

- ✅ 使用 libvgm 渲染 ADPCM（100% 准确）
- ✅ FM 音量自动降低为 1/8（避免削波）
- ✅ 正确的 DAC 立体声设置
- ✅ 保留原始循环点

### VGM 格式

- ✅ 完全符合 VGM 1.51 规范
- ✅ 通过 VGM 验证器测试
- ✅ 支持 GD3 标签（元数据）
- ✅ 可压缩为 .vgz 格式

### 转换规则

1. **FM 通道**: YM2610 的 4 个 FM 通道 → YM2612 的 4 个 FM 通道
2. **ADPCM**: ADPCM-A (6通道) + ADPCM-B (1通道) → YM2612 DAC
3. **SSG**: 舍弃（YM2612 不支持）
4. **时钟**: 保持 8 MHz

## 已知限制

1. **文件大小**: 输出文件较大（5-8MB），因为 DAC 流未压缩
   - 解决方案：使用 gzip 压缩为 .vgz 格式

2. **SSG 通道**: 不支持 YM2610 的 SSG (PSG) 通道
   - 原因：YM2612 没有 SSG 功能

3. **采样率**: DAC 采样率固定为 44.1kHz
   - 与 VGM 标准采样率一致

## 系统要求

- **操作系统**: Windows 7 或更高版本
- **架构**: 64-bit 推荐
- **内存**: 至少 512 MB RAM
- **磁盘空间**: 每个转换文件需要 5-10 MB

## 构建说明

如需从源码构建：

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 前置要求

- CMake 3.12+
- C++11 编译器 (GCC/MSVC)
- C99 编译器

## 版本历史

### v1.0 (2026-02-04)

- ✅ 首次正式发布
- ✅ 完整的 FM 通道转换
- ✅ 使用 libvgm 渲染 ADPCM
- ✅ 自动 DAC 立体声设置
- ✅ 批量转换支持
- ✅ ZIP/VGZ 格式支持

## 技术支持

如有问题或建议，请：

1. 查看 docs/ 文件夹中的详细文档
2. 检查 README.md 中的常见问题
3. 提交 issue 到项目仓库

## 许可证

本项目使用 libvgm 库，遵循其许可证条款。

## 致谢

- libvgm 项目 - 提供核心音频模拟
- VGM 社区 - 格式规范和工具

---

**下载**: YM2610_to_YM2612_v1.0_Release.tar.gz (1.9 MB)
**解压后大小**: 7.6 MB
**包含**: 可执行文件 + 源代码 + 文档 + 依赖库
