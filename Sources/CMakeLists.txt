cmake_minimum_required(VERSION 3.0)
project(AmigaCore)

file( GLOB SRCS
	"*.cpp"
	)

file( GLOB HEADERS
	"*.h"
)

set(EXTRA
   TODO.txt
	"../Notes/DMA.txt"
   "../Notes/Kickstart 1.2.asm"
   )

source_group("Docs" FILES ${EXTRA} )

add_library(AmigaCore ${SRCS} ${HEADERS} ${EXTRA})

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
