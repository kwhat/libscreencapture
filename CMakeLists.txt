# libScreenCapture: Cross-platform screen capture library.
# Copyright (C) 2006-2020 Alexander Barker.  All Rights Received.
# https://github.com/kwhat/libscreencapture/
#
# libScreenCapture is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# UIOHook is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)

project(screencapture VERSION 0.1.0 LANGUAGES C)


if (WIN32 OR WIN64)
    set(SCREEN_CAPTURE_SOURCE_DIR "windows")
elseif (APPLE)
    set(SCREEN_CAPTURE_SOURCE_DIR "darwin")
else()
    set(SCREEN_CAPTURE_SOURCE_DIR "x11")
endif()

add_library(screencapture
    "src/${SCREEN_CAPTURE_SOURCE_DIR}/screen_capture.c"
    "src/${SCREEN_CAPTURE_SOURCE_DIR}/screen_info.c"
    "src/logging.c"
)

set_target_properties(screencapture PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE 1
    OUTPUT_NAME "${PROJECT_NAME}"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/screencapture.h
)

include(GNUInstallDirs)
target_include_directories(screencapture
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${SCREEN_CAPTURE_SOURCE_DIR}
)


install(TARGETS screencapture
    EXPORT ${PROJECT_NAME}-config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

export(TARGETS screencapture FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake")
install(EXPORT ${PROJECT_NAME}-config DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


if(BUILD_DEMO)
    if (NOT BUILD_SHARED_LIBS)
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    endif()

    add_executable(demo_capture
        "./demo/demo_capture.c"
        #"./demo/demo_info.c"
    )
    add_dependencies(demo_capture screencapture)
    target_link_libraries(demo_capture screencapture "${CMAKE_THREAD_LIBS_INIT}")

    set_target_properties(demo_capture PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
    )

    if(MSVC)
        add_compile_definitions(demo_capture PRIVATE inline=__inline)
        add_compile_definitions(demo_capture PRIVATE _CRT_SECURE_NO_WARNINGS)

        if (MSVC_VERSION LESS "1900")
            add_compile_definitions(demo_capture PRIVATE snprintf=_snprintf)
        endif()
    endif()

    install(TARGETS demo_capture RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()


if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(X11 REQUIRED x11)
    target_include_directories(screencapture PRIVATE "${X11_INCLUDE_DIRS}")
    target_link_libraries(screencapture "${X11_LDFLAGS}")

    pkg_check_modules(JPEG REQUIRED libjpeg)
    target_include_directories(demo_capture PRIVATE "${JPEG_INCLUDE_DIRS}")
    target_link_libraries(demo_capture "${JPEG_LDFLAGS}")

    option(USE_XSHM "X Shared Memory Extension (default: ON)" ON)
    if(USE_XSHM)
        pkg_check_modules(XEXT REQUIRED xext)
        add_compile_definitions(screencapture PRIVATE USE_XSHM)
        target_include_directories(screencapture PRIVATE "${XEXT_INCLUDE_DIRS}")
        target_link_libraries(screencapture "${XEXT_LDFLAGS}")
    endif()

    option(USE_XRANDR "XRandR Extension (default: OFF)" OFF)
    if(USE_XRANDR)
        pkg_check_modules(XRANDR REQUIRED xrandr)
        add_compile_definitions(screencapture PRIVATE USE_XRANDR)
        target_include_directories(screencapture PRIVATE "${XRANDR_INCLUDE_DIRS}")
        target_link_libraries(screencapture "${XRANDR_LDFLAGS}")
    endif()

    option(USE_XINERAMA "Xinerama Extension (default: ON)" ON)
    if(USE_XINERAMA)
        pkg_check_modules(XINERAMA REQUIRED xinerama)
        add_compile_definitions(screencapture PRIVATE USE_XINERAMA)
        target_include_directories(screencapture PRIVATE "${XINERAMA_INCLUDE_DIRS}")
        target_link_libraries(screencapture "${XINERAMA_LDFLAGS}")
    endif()
elseif(APPLE)
    message(STATUS "Is Apple")
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.5")

    find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
    target_include_directories(demo_capture PRIVATE "${CORE_FOUNDATION}")
    target_link_libraries(demo_capture "${CORE_FOUNDATION}")

    find_library(CORE_GRAPHICS CoreGraphics REQUIRED)
    target_include_directories(demo_capture PRIVATE "${CORE_GRAPHICS}")
    target_link_libraries(demo_capture "${CORE_GRAPHICS}")

    find_library(CORE_SERVICES CoreServices REQUIRED)
    target_include_directories(demo_capture PRIVATE "${CORE_SERVICES}")
    target_link_libraries(demo_capture "${CORE_SERVICES}")

    find_library(IMAGE_IO ImageIO REQUIRED)
    target_include_directories(demo_capture PRIVATE "${IMAGE_IO}")
    target_link_libraries(demo_capture "${IMAGE_IO}")
elseif(WIN32)
    #find_library(ADVAPI32 Advapi32)
    #target_include_directories(screencapture PRIVATE "${ADVAPI32}")
    #target_link_libraries(screencapture "${ADVAPI32}")

    #find_file(ADVAPI32 Advapi32)
    #add_library(ADVAPI32 Advapi32.dll)
    #set_target_properties(ADVAPI32 PROPERTIES LINKER_LANGUAGE C)
    #target_link_libraries(screencapture "${ADVAPI32}")
endif()


list(REMOVE_DUPLICATES INTERFACE_LINK_LIBRARIES)
string(REPLACE ";" " " COMPILE_LIBRARIES "${INTERFACE_LINK_LIBRARIES}")
configure_file("pc/screencapture.pc.in" "${PROJECT_BINARY_DIR}/screencapture.pc" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/screencapture.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
