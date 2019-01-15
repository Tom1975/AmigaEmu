cmake_minimum_required(VERSION 3.0)

# Nom du projet
project(AmigaCore)

set (gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set (CMAKE_INSTALL_PREFIX ${AmigaCore_BINARY_DIR}/Install)
set (SKIP_INSTALL_ALL ON)

add_subdirectory(Sources)
add_subdirectory(googletest)
add_subdirectory(Tests)
add_subdirectory(CuteAmiga)
