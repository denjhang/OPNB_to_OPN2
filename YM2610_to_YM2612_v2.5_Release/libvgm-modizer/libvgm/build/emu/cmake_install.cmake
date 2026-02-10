# Install script for directory: /d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/mingw64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/build/bin/libvgm-emu.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vgm/emu" TYPE FILE FILES
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/snddef.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/EmuStructs.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/SoundEmu.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/SoundDevs.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/EmuCores.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/Resampler.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/logging.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/dac_control.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vgm/emu/cores" TYPE FILE FILES
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/sn764intf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/2413intf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/2612intf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/ym2151.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/2151intf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/segapcm.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/rf5cintf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/opnintf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/opnintf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/oplintf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/oplintf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/oplintf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/262intf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/ymf278b.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/ymz280b.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/ymf271.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/ayintf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/pwm.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/gb.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/nesintf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/multipcm.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/upd7759.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/okim6258.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/okim6295.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/k051649.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/k054539.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/c6280intf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/c140.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/c219.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/k053260.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/pokey.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/qsoundintf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/scsp.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/ws_audio.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/vsu.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/saaintf.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/es5503.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/es5506.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/x1_010.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/c352.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/iremga20.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/emu/cores/mikey.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/build/emu/vgm-emu.pc")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/build/emu/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
