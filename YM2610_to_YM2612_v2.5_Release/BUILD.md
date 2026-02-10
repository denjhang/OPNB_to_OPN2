# YM2610 to YM2612 Converter v2.5 - 编译说明

## 系统要求

- Windows with MSYS2/MinGW64
- CMake 3.12 或更高版本
- GCC/G++ 编译器
- zlib 库

## 编译步骤

### 1. 准备环境

确保已安装MSYS2/MinGW64环境：

```bash
# 检查工具版本
cmake --version
gcc --version
g++ --version
```

### 2. 准备依赖库

依赖库已包含在发布包中：

```
libvgm-modizer/libvgm/
├── build/bin/          # 预编译的.a库文件
├── player/             # 头文件
├── emu/                # 头文件
└── utils/              # 头文件
```

### 3. 编译主程序

```bash
cd 00_source
mkdir build
cd build

# 创建lib目录并复制依赖库
mkdir lib
cp ../../libvgm-modizer/libvgm/build/bin/*.a lib/

# 配置CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release
```

### 4. 验证编译结果

编译成功后，build目录应包含以下文件：

```
build/
├── vgm_converter.exe           # FM通道转换器 (约189KB)
└── vgm2wav_adpcm_only.exe     # ADPCM提取器 (约2.2MB)
```

### 5. 测试

```bash
# 返回发布包根目录
cd ../..

# 测试转换
./convert_complete.sh examples/input/test.vgm examples/output/test_YM2612.vgm
```

## 编译选项

### Release模式 (推荐)
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```
- 优化性能
- 文件体积较小
- 适合发布使用

### Debug模式
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```
- 包含调试信息
- 文件体积较大
- 适合开发调试

## 常见问题

### 问题1: 找不到libvgm头文件

**错误信息**:
```
fatal error: player/playerbase.hpp: No such file or directory
```

**解决方法**:
确保libvgm目录结构正确，并且CMakeLists.txt中的路径设置正确：

```cmake
target_include_directories(vgm_converter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libvgm
)
```

### 问题2: 链接错误

**错误信息**:
```
undefined reference to `PlayerBase::Start()'
```

**解决方法**:
确保已复制所有.a库文件到build/lib目录：

```bash
cp ../../libvgm-modizer/libvgm/build/bin/*.a lib/
```

### 问题3: zlib库缺失

**错误信息**:
```
cannot find -lz
```

**解决方法**:
安装zlib库：

```bash
pacman -S mingw-w64-x86_64-zlib
```

## 源代码结构

```
00_source/
├── src/
│   ├── main.cpp                    # 主程序 (vgm_converter)
│   ├── CommandMapper.cpp           # FM命令映射
│   ├── CommandMapper.h             # 命令映射头文件
│   ├── VGMReader.h                 # VGM读取器
│   ├── VGMWriter.h                 # VGM写入器
│   ├── VGMValidator.h              # VGM验证器
│   ├── vgm2wav_adpcm_only.cpp     # ADPCM提取器
│   └── ...
├── CMakeLists.txt                  # CMake配置
└── build/                          # 编译输出目录
```

## 修改和定制

### 调整FM音量

编辑 `src/CommandMapper.cpp`:

```cpp
UINT8 CommandMapper::AdjustTL(UINT8 tl) {
    tl = tl & 0x7F;

    // 修改这里的数值 (当前是+16)
    return (tl + 16 > 127) ? 127 : tl + 16;
}
```

### 调整PCM音量

编辑 `src/vgm2wav_adpcm_only.cpp`:

```cpp
// 修改这里的数值
// 0x10000 = 100%
// 0x18000 = 150% (当前设置)
// 0x20000 = 200%
pCfg.masterVol = 0x18000;
```

### 重新编译

```bash
cd 00_source/build
cmake --build . --config Release
```

## 性能优化

### 编译器优化选项

在CMakeLists.txt中添加：

```cmake
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
endif()
```

### 链接时优化 (LTO)

```cmake
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
```

## 交叉编译

### 为其他平台编译

```bash
# 为32位Windows编译
cmake .. -DCMAKE_TOOLCHAIN_FILE=toolchain-mingw32.cmake

# 为Linux编译 (需要交叉编译工具链)
cmake .. -DCMAKE_TOOLCHAIN_FILE=toolchain-linux.cmake
```

## 清理编译文件

```bash
cd 00_source/build
rm -rf *
```

## 技术支持

如遇到编译问题，请检查：

1. MSYS2/MinGW64环境是否正确安装
2. CMake版本是否满足要求 (3.12+)
3. 依赖库文件是否完整
4. 路径设置是否正确

---

编译说明文档
YM2610 to YM2612 Converter v2.5
© 2026
