# YM2610 to YM2612 Converter v2.5 - 版本更新说明

## v2.5 (2026-02-10)

### 主要更新

#### 🎯 核心改进
- **混合最佳实践**: 结合v2.0的稳定性和v2.3的载波检测技术
- **TL调整方法**: 使用v2.0验证过的TL+16简单加法方法
- **载波检测**: 保留v2.3的载波运算符识别功能
- **音量优化**: PCM音量调整为150%，提高硬件兼容性

#### ✨ 新功能
- 只调整载波运算符的TL值，保持音色完整性
- 基于FM算法的智能载波检测 (8种算法支持)
- 改进的FM6→FM4通道映射
- 完整的GD3标签保留

#### 🔧 技术改进
- TL寄存器正确屏蔽第7位 (0x7F)
- 优化的算法跟踪 (0xB0-0xB2寄存器)
- 改进的DAC采样率转换
- 更稳定的硬件兼容性

### 测试状态

✅ **大规模测试完成**
- 转换游戏数: 48+个
- VGM文件数: 1100+个
- 总数据量: 约10GB
- 测试平台: Windows (MSYS2/MinGW64)

#### 测试游戏列表

**KOF系列 (8个)**:
- The King of Fighters '96 (22 files, 279MB)
- The King of Fighters '97 (37 files, 377MB)
- The King of Fighters '98 (42 files, 409MB)
- The King of Fighters '99 (31 files, 338MB)
- The King of Fighters 2000 (33 files, 587MB)
- The King of Fighters 2001 (29 files, 200MB)
- The King of Fighters 2002 (23 files, 268MB)
- The King of Fighters 2003 (29 files, 212MB)

**其他游戏 (40+个)**:
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

### 版本对比

| 特性 | v2.0 | v2.3 | v2.4 | v2.5 |
|------|------|------|------|------|
| FM映射 | FM6→FM1 | FM6→FM4 | FM6→FM4 | FM6→FM4 |
| 工作FM通道 | 3个 | 4个 | 4个 | 4个 |
| FM音量方法 | TL+16 | TL×2.5 | TL+16 | TL+16 |
| 调整范围 | 全部OP | 仅载波 | 全部OP | 仅载波 |
| 载波检测 | 否 | 是 | 否 | 是 |
| PCM音量 | 150% | 200% | 150% | 150% |
| 硬件兼容性 | FM缺失 | PCM干扰 | 良好 | 最佳 |

### 技术细节

#### TL寄存器处理
```cpp
UINT8 CommandMapper::AdjustTL(UINT8 tl) {
    // v2.5: 使用v2.0的简单加法 (+16) 配合v2.3的载波检测
    tl = tl & 0x7F;  // 屏蔽第7位
    return (tl + 16 > 127) ? 127 : tl + 16;
}
```

#### 载波运算符表
```cpp
static const bool carrierTable[8][4] = {
    {false, false, false, true},   // ALGO 0: 仅OP4
    {false, false, false, true},   // ALGO 1: 仅OP4
    {false, false, false, true},   // ALGO 2: 仅OP4
    {false, false, false, true},   // ALGO 3: 仅OP4
    {false, false, true, true},    // ALGO 4: OP2+OP4
    {false, true, true, true},     // ALGO 5: OP3+OP2+OP4
    {false, true, true, true},     // ALGO 6: OP3+OP2+OP4
    {true, true, true, true}       // ALGO 7: 全部
};
```

### 已知问题修复

#### 修复1: TL寄存器溢出
- **问题**: v2.3的TL乘法导致值溢出
- **解决**: 使用v2.0的TL+16加法方法
- **影响**: FM通道音量正常

#### 修复2: PCM硬件干扰
- **问题**: v2.3的200% PCM音量在硬件上造成干扰
- **解决**: 降低到150% PCM音量
- **影响**: 硬件播放稳定

#### 修复3: FM通道缺失
- **问题**: v2.0的FM6→FM1映射导致FM1被覆盖
- **解决**: 使用v2.3的FM6→FM4映射
- **影响**: 4个FM通道正常工作

### 性能优化

- 优化DAC采样率转换算法
- 减少临时文件I/O操作
- 改进内存使用效率
- 加快VGM文件解析速度

### 文档更新

- 新增完整的README.md
- 新增BUILD.md编译说明
- 新增RELEASE_NOTES.md (本文件)
- 新增VERSION.txt版本信息
- 改进代码注释

### 下一步计划

- [ ] 支持更多YM2610变体
- [ ] 添加GUI界面
- [ ] 支持批量转换
- [ ] 添加音量预览功能
- [ ] 支持自定义音量参数

---

## v2.4 (2026-02-09)

### 主要更新
- 使用v2.3的FM6→FM4映射
- 使用v2.0的TL+16音量调整
- PCM音量150%

### 技术改进
- 结合v2.0和v2.3的优点
- 移除载波检测以简化代码
- 保持硬件兼容性

---

## v2.3 (2026-02-09)

### 主要更新
- 实现载波运算符检测
- 只调整载波运算符的TL值
- 修复TL寄存器溢出问题

### 新功能
- 基于FM算法的载波检测
- 8种FM算法支持
- 算法跟踪功能

### 已知问题
- PCM音量200%在硬件上可能造成干扰
- TL乘法方法可能导致音量过小

---

## v2.0 (2026-02-08)

### 初始版本
- 基本FM通道转换
- ADPCM转DAC功能
- FM6→FM1映射
- TL+16音量调整
- PCM音量150%

### 已知问题
- FM6→FM1映射导致FM1被覆盖
- 只有3个FM通道工作

---

YM2610 to YM2612 Converter v2.5
© 2026
