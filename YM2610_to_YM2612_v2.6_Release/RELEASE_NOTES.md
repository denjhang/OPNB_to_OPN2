# YM2610 to YM2612 Converter - Release Notes

## v2.6 (2026-02-10) - Super_Dodge_Ball优化版

### 🎯 版本目标
专门为Super_Dodge_Ball等需要更强FM音量降低的游戏优化。

### ✨ 新特性
- **强化FM音量降低**: 采用TL×2.5方法，将FM音量降低到40%
- **保持v2.5架构**: 继承v2.5的稳定性和载波检测功能
- **专门优化**: 针对FM音轨过响的游戏

### 🔧 技术变更

#### FM音量调整
```cpp
// v2.5: TL+16 (降低约15%)
return (tl + 16 > 127) ? 127 : tl + 16;

// v2.6: TL×2.5 (降低到40%)
UINT16 adjusted = (tl * 5) / 2;
return (adjusted > 127) ? 127 : (UINT8)adjusted;
```

#### 保持不变
- FM通道映射: FM6 → FM4
- 载波运算符检测: 8种算法支持
- PCM音量: 150%
- 仅调整载波运算符

### 📊 测试结果

#### Super_Dodge_Ball转换
- **文件数量**: 16个VGM文件
- **总大小**: 160MB
- **FM命令**: 6603+ (01 Title)
- **状态**: ✅ 转换成功

#### 文件列表
1. 00 Neo Geo Logo (1.6MB)
2. 01 Title (4.9MB)
3. 02 Team Select (4.1MB)
4. 03 Kunio Team Stage (16MB)
5. 04 Winning Demo (2.4MB)
6. 05 Riki Team Stage (9.8MB)
7. 06 Shinji Team Stage (16MB)
8. 07 Misuzu Team Stage (17MB)
9. 08 Sabu Team Stage (14MB)
10. 09 Miyuki Team Stage (13MB)
11. 10 Kenji Team Stage (15MB)
12. 11 Maou Team Stage (13MB)
13. 12 Ending (13MB)
14. 13 Credits Roll (14MB)
15. 14 Continue (8.0MB)
16. 15 Game Over (1.1MB)

### 🆚 版本对比

| 特性 | v2.5 | v2.6 |
|------|------|------|
| **FM音量方法** | TL+16 | TL×2.5 |
| **音量效果** | 降低约15% | 降低到40% |
| **FM映射** | FM6→FM4 | FM6→FM4 |
| **载波检测** | 是 | 是 |
| **PCM音量** | 150% | 150% |
| **适用场景** | 大多数游戏 | Super_Dodge_Ball等 |
| **硬件兼容** | 验证通过 | 基于v2.5 |

### 📝 使用建议

#### 选择v2.5的情况
- Money_Puzzle_Exchanger ✅
- KOF系列 (96, 97, 98, 99, 2000, 2001, 2002, 2003)
- 大多数Neo Geo游戏
- FM/PCM平衡良好的游戏

#### 选择v2.6的情况
- Super_Dodge_Ball ⚠️
- FM音轨明显过响的游戏
- 需要更强音量降低的场景
- v2.5转换后FM过响的游戏

### 🔍 技术细节

#### TL寄存器处理
- **位宽**: 7位 (0-127)
- **特性**: TL值越大，音量越小
- **v2.6方法**: TL×2.5
  - 原始TL=20 → 调整后TL=50 (音量降低到40%)
  - 原始TL=50 → 调整后TL=125 (音量降低到40%)
  - 原始TL=51+ → 调整后TL=127 (限幅)

#### 载波运算符表
```
算法 0: OP4
算法 1: OP4
算法 2: OP4
算法 3: OP4
算法 4: OP2 + OP4
算法 5: OP3 + OP2 + OP4
算法 6: OP3 + OP2 + OP4
算法 7: OP1 + OP3 + OP2 + OP4
```

只调整载波运算符，保持调制器不变，最小化音色失真。

### 📦 发布内容

#### 可执行文件
- `vgm_converter.exe` (351KB) - FM通道转换器
- `vgm2wav_adpcm_only.exe` (2.2MB) - ADPCM提取器

#### 源代码
- `00_source/src/` - 完整C++源代码
- `00_source/CMakeLists.txt` - CMake配置

#### 依赖库
- `libvgm-modizer/libvgm/` - libvgm库 (34MB)

#### 脚本
- `convert_complete.sh` - 批量转换脚本

#### 文档
- `README.md` - 使用说明
- `BUILD.md` - 编译说明
- `RELEASE_NOTES.md` - 本文件
- `VERSION.txt` - 版本信息

#### 示例文件
- `examples/output/` - Super_Dodge_Ball转换示例

### 🐛 已知问题

1. **SSG通道丢失**: YM2610的SSG通道无法转换到YM2612
2. **FM4通道覆盖**: FM4被FM6覆盖，实际只有4个FM通道
3. **文件大小**: DAC数据未压缩，文件较大
4. **音量平衡**: 不同游戏可能需要不同的音量设置

### 🔄 从v2.5升级

如果您已经使用v2.5转换了游戏，并发现FM音轨过响：
1. 使用v2.6重新转换
2. 对比v2.5和v2.6的输出
3. 选择音量平衡更好的版本

### 💡 自定义音量

如需调整FM音量，修改 `CommandMapper.cpp`:

```cpp
// 更强降低 (TL×3 = 音量降低到33%)
UINT16 adjusted = tl * 3;

// 中等降低 (TL×2 = 音量降低到50%)
UINT16 adjusted = tl * 2;

// 轻度降低 (TL×1.5 = 音量降低到67%)
UINT16 adjusted = (tl * 3) / 2;
```

然后重新编译：
```bash
cd 00_source/build
cmake --build . --config Release
```

### 📊 性能指标

- **编译时间**: ~30秒 (Release模式)
- **转换速度**: ~1-2秒/文件 (取决于文件大小)
- **内存使用**: <100MB
- **可执行文件**: 2.5MB (总计)

### 🙏 致谢

- libvgm项目提供VGM播放和处理库
- Neo Geo和Sega Genesis社区的技术文档
- 测试和反馈的用户

---

## 版本历史

### v2.6 (2026-02-10)
- 强化FM音量降低 (TL×2.5)
- 专门优化Super_Dodge_Ball
- 基于v2.5稳定架构

### v2.5 (2026-02-10)
- 混合v2.0和v2.3的最佳实践
- TL+16音量调整
- 载波运算符检测
- 大规模转换测试 (48+游戏, 1100+文件)

### v2.4 (2026-02-09)
- 简化版本
- TL+16方法
- 调整所有运算符

### v2.3 (2026-02-09)
- 载波运算符检测
- TL×2.5方法
- 8种FM算法支持

### v2.0 (2026-02-08)
- 初始版本
- 基本FM转换
- ADPCM转DAC

---

YM2610 to YM2612 VGM Converter v2.6
© 2026
