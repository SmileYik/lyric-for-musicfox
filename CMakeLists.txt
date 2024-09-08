cmake_minimum_required(VERSION 3.5)

project(lyric-for-musicfox VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 修改歌词通信时使用的UDP端口
set(CONFIG_PORT 16501)

# add_definitions(-DDEBUG_FLAG)
add_definitions(-DCONFIG_PORT=${CONFIG_PORT})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        src/setting.h
        src/setting.cpp
        src/settingwindow.h
        src/settingwindow.cpp
        src/settingwindow.ui
        src/lyricwidget.h
        src/lyricwidget.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(lyric-for-musicfox
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET lyric-for-musicfox APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(lyric-for-musicfox SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(lyric-for-musicfox
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(lyric-for-musicfox PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

if(MSVC)
    message("msvc")
    add_compile_options("/permissive-")
    add_compile_options("/Zc:__cplusplus")
    add_compile_options("/MT")
    set_target_properties(lyric-for-musicfox PROPERTIES CXX_STANDARD 17)
    target_compile_options(lyric-for-musicfox PUBLIC "/Zc:__cplusplus" PUBLIC "/permissive-" PUBLIC "/MT")
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.lyric-for-musicfox)
endif()
set_target_properties(lyric-for-musicfox PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS lyric-for-musicfox
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(lyric-for-musicfox)
endif()

# 生成带有自定端口的补丁
set(PATCH_CONTENT)
file(STRINGS musicfox.patch PATCH_CONTENT NEWLINE_CONSUME ENCODING UTF-8)
string(REPLACE ";" "\${split}" PATCH_CONTENT ${PATCH_CONTENT})
string(REPLACE "\${CONFIG_PORT}" "${CONFIG_PORT}" PATCH_CONTENT ${PATCH_CONTENT})
string(REPLACE "\${split}" "\;" PATCH_CONTENT ${PATCH_CONTENT})
file(WRITE musicfox-target-port.patch ${PATCH_CONTENT})
# 生成带有自定端口的补丁 结束

add_custom_target(clone-musicfox
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} "-E" "remove_directory" "go-musicfox"
    COMMAND git clone --recursive "https://github.com/go-musicfox/go-musicfox.git"
    COMMENT "Cloning musicfox"
    USES_TERMINAL
)

add_custom_target(patch-musicfox
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/go-musicfox"
    COMMAND git apply "${CMAKE_CURRENT_SOURCE_DIR}/musicfox-target-port.patch"
    COMMENT "Patching..."
    USES_TERMINAL
)

add_custom_target(musicfox
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/go-musicfox"
    COMMAND make
    COMMENT "Building musicfox..."
    USES_TERMINAL
)

add_dependencies(patch-musicfox clone-musicfox)
add_dependencies(musicfox patch-musicfox)
add_dependencies(lyric-for-musicfox musicfox)
