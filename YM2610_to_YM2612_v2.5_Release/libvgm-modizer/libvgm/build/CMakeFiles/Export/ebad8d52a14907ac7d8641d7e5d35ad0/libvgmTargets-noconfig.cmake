#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libvgm::vgm-utils" for configuration ""
set_property(TARGET libvgm::vgm-utils APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(libvgm::vgm-utils PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libvgm-utils.a"
  )

list(APPEND _cmake_import_check_targets libvgm::vgm-utils )
list(APPEND _cmake_import_check_files_for_libvgm::vgm-utils "${_IMPORT_PREFIX}/lib/libvgm-utils.a" )

# Import target "libvgm::vgm-audio" for configuration ""
set_property(TARGET libvgm::vgm-audio APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(libvgm::vgm-audio PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C;CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libvgm-audio.a"
  )

list(APPEND _cmake_import_check_targets libvgm::vgm-audio )
list(APPEND _cmake_import_check_files_for_libvgm::vgm-audio "${_IMPORT_PREFIX}/lib/libvgm-audio.a" )

# Import target "libvgm::vgm-emu" for configuration ""
set_property(TARGET libvgm::vgm-emu APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(libvgm::vgm-emu PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libvgm-emu.a"
  )

list(APPEND _cmake_import_check_targets libvgm::vgm-emu )
list(APPEND _cmake_import_check_files_for_libvgm::vgm-emu "${_IMPORT_PREFIX}/lib/libvgm-emu.a" )

# Import target "libvgm::vgm-player" for configuration ""
set_property(TARGET libvgm::vgm-player APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(libvgm::vgm-player PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C;CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libvgm-player.a"
  )

list(APPEND _cmake_import_check_targets libvgm::vgm-player )
list(APPEND _cmake_import_check_files_for_libvgm::vgm-player "${_IMPORT_PREFIX}/lib/libvgm-player.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
