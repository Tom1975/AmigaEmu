cmake_minimum_required(VERSION 3.0)

# Nom du projet
project(AmigaCore)

#no config util
set ( ALSOFT_NO_CONFIG_UTIL ON CACHE BOOL "" FORCE)
set ( ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set ( ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set ( ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set ( ALSOFT_CONFIG OFF CACHE BOOL "" FORCE)
set ( ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set ( ALSOFT_HRTF_DEFS OFF CACHE BOOL "" FORCE)
set ( ALSOFT_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE   )

set (gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set (CMAKE_INSTALL_PREFIX ${AmigaCore_BINARY_DIR}/Install)
set (SKIP_INSTALL_ALL ON)

add_subdirectory(Sources)
add_subdirectory(googletest)
add_subdirectory(Tests)
add_subdirectory(openal-soft)
add_subdirectory(CuteAmiga)
