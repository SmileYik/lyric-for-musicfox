cmake_minimum_required(VERSION 3.5)

project(lyric-for-musicfox VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 修改歌词通信时使用的UDP端口
set(CONFIG_PORT 16501)
# 跳过编译 musicfox
if(NOT DEFINED SKIP_BUILD_MUSICFOX)
    set(SKIP_BUILD_MUSICFOX NO)
endif()

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

set(DBUS_MPRIS_SOURCES
        src/dbus-mode/musicfoxmanager.h
        src/dbus-mode/musicfoxmanager.cpp
        src/dbus-mode/mprismetadata.h
        src/dbus-mode/mprismetadata.cpp
        src/dbus-mode/lyric.h
        src/dbus-mode/lyric.cpp
        src/dbus-mode/qtlistener.h
        src/dbus-mode/qtlistener.cpp
        src/dbus-mode/lyriccontroller.h
        src/dbus-mode/lyriccontroller.cpp
        src/dbus-mode/lyricnetworkcontroller.h
        src/dbus-mode/lyricnetworkcontroller.cpp
)

set(MPRIS_LYRIC_SENDER_SOURCES
        src/mpris-lyric-sender.cpp
        ${DBUS_MPRIS_SOURCES}
)

set(PROJECT_INCLUDE_DIRECTORIES)

set(TARGET_LINK_LIBRARY_LIST
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)
set(MPRIS_LYRIC_SENDER_TARGET_LINK_LIBRARY_LIST
    Qt${QT_VERSION_MAJOR}::Network
)

if(UNIX)
    message("-- * enable mpris")
    list(APPEND PROJECT_SOURCES ${DBUS_MPRIS_SOURCES})
    find_package(DBus1 REQUIRED)
    list(APPEND TARGET_LINK_LIBRARY_LIST dbus-1)
    list(APPEND MPRIS_LYRIC_SENDER_TARGET_LINK_LIBRARY_LIST dbus-1)
    add_definitions(-DLINUX)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(lyric-for-musicfox
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    if(UNIX)
        qt_add_executable(mpris-lyric-sender
            MANUAL_FINALIZATION
            ${MPRIS_LYRIC_SENDER_SOURCES}
        )
    endif()
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
        if(UNIX)
            add_executable(mpris-lyric-sender ${MPRIS_LYRIC_SENDER_SOURCES})
        endif()
    endif()
endif()

target_include_directories(lyric-for-musicfox PRIVATE ${PROJECT_INCLUDE_DIRECTORIES})
target_link_libraries(lyric-for-musicfox PRIVATE ${TARGET_LINK_LIBRARY_LIST})

if(UNIX)
    target_include_directories(mpris-lyric-sender PRIVATE ${PROJECT_INCLUDE_DIRECTORIES})
    target_link_libraries(mpris-lyric-sender PRIVATE ${MPRIS_LYRIC_SENDER_TARGET_LINK_LIBRARY_LIST})
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
    if(UNIX)
        qt_finalize_executable(mpris-lyric-sender)
    endif()
    
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

if(SKIP_BUILD_MUSICFOX)
    message("-- * Skip build patched musicfox.")
else()
    add_dependencies(lyric-for-musicfox musicfox)
endif()

if(DEBUG_FLAG)
    message("-- * Enable debug log")
    add_definitions(-DDEBUG_FLAG)
endif()

if(MSVC)
    message("-- * msvc setting")
    add_compile_options("/permissive-")
    add_compile_options("/Zc:__cplusplus")
    add_compile_options("/MT")
    set_target_properties(lyric-for-musicfox PROPERTIES CXX_STANDARD 17)
    target_compile_options(lyric-for-musicfox PUBLIC "/Zc:__cplusplus" PUBLIC "/permissive-" PUBLIC "/MT")
endif()
