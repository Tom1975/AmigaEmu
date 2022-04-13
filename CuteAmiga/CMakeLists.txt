cmake_minimum_required(VERSION 3.8)
project(CuteAmiga)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories( ${AmigaCore} )

find_package(Qt5Widgets REQUIRED)

qt5_add_resources(QRCS resources.qrc)

file( GLOB SRCS
	"*.cpp"
	)
file( GLOB HEADERS
	"*.h"
)


set ( UIS
 BitplaneDialog.ui
 CopperDialog.ui
 DebugDialog.ui
 DiskDialog.ui
 ExecDialog.ui  
 mainwindow.ui
 MemoryDialog.ui
)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../Sources )

if(WIN32)
  set(CMAKE_WIN32_EXECUTABLE ON)
endif()

add_definitions(-DWIN32 )
set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DSFML_STATIC ")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /DSFML_STATIC ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DSFML_STATIC ")

qt5_add_resources(QRCS resources.qrc)

add_executable (CuteAmiga WIN32 ${SRCS} ${HEADERS} ${UI_HEADERS} ${UIS} ${QRCS})

target_link_libraries(CuteAmiga ${Qt5Widgets_LIBRARIES} Qt5::Core Qt5::Widgets 
						AmigaCore
						)

#if(WIN32)
    #include(Windeployqt)
    #windeployqt(CuteAmiga ${INSTALL_BIN_PATH})
#endif()