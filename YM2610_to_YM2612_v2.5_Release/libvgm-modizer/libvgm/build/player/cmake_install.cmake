# Install script for directory: /d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/player

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/build/bin/libvgm-player.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vgm/player" TYPE FILE FILES
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/player/dblk_compr.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/player/helper.h"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/player/playerbase.hpp"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/player/droplayer.hpp"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/player/gymplayer.hpp"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/player/s98player.hpp"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/player/vgmplayer.hpp"
    "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/player/playera.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/build/player/vgm-player.pc")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/d/working/vscode-projects/modizer-libs/libvgm-modizer/libvgm/build/player/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
