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

if(Qt5_FOUND AND WIN32 AND TARGET Qt5::qmake AND NOT TARGET Qt5::windeployqt)
    get_target_property(_qt5_qmake_location Qt5::qmake IMPORTED_LOCATION)

    execute_process(
        COMMAND "${_qt5_qmake_location}" -query QT_INSTALL_PREFIX
        RESULT_VARIABLE return_code
        OUTPUT_VARIABLE qt5_install_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(imported_location "${qt5_install_prefix}/bin/windeployqt.exe")

    if(EXISTS ${imported_location})
        add_executable(Qt5::windeployqt IMPORTED)

        set_target_properties(Qt5::windeployqt PROPERTIES
            IMPORTED_LOCATION ${imported_location}
        )
    endif()
endif()

get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

function(windeployqt target)

    # POST_BUILD step
    # - after build, we have a bin/lib for analyzing qt dependencies
    # - we run windeployqt on target and deploy Qt libs

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${_qt_bin_dir}/windeployqt.exe"         
                \"$<TARGET_FILE:${target}>\"
        COMMENT "Deploying Qt libraries using windeployqt for compilation target '${target}' ..."
    )

endfunction()

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

target_compile_features(CuteAmiga PRIVATE cxx_std_17)

target_link_libraries(CuteAmiga ${Qt5Widgets_LIBRARIES} OpenAL Qt5::Core Qt5::Widgets 
						AmigaCore
						)

#if(WIN32)
    #include(Windeployqt)
    #windeployqt(CuteAmiga ${INSTALL_BIN_PATH})
#endif()