cmake_minimum_required(VERSION 3.0)

project(Tests)
enable_testing()

include_directories( ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${AmigaCore})

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
		_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING 
        )

set(SRCS
	test.cpp
	TestEngineCpu.cpp
	)

set(HEADERS
	TestEngineCpu.h
)

add_executable( Tests   ${SRCS} ${HEADERS} )
include_directories(${CMAKE_CURRENT_LIST_DIR}/../Sources )

# Link test executable against gtest & gtest_main
target_include_directories( Tests PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../Sources )
target_link_libraries(Tests gtest gtest_main AmigaCore )

add_test( NAME Tests COMMAND Tests)

if(CMAKE_COMPILER_IS_GNUCC)
	set ( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --std=c++1z )
endif()

set ( CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} )

if(MSVC)
	target_link_libraries ( Tests )
else ()
	target_link_libraries ( Tests  m stdc++fs )
endif()

