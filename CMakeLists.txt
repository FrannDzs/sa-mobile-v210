cmake_minimum_required(VERSION 3.22)

option(SAMOBILE_OUTRELDIR "Path to the local installation directory")
option(SAMOBILE_SHARED_NAME "Name of the output core library")
option(SAMOBILE_VERSION "Implementation version")

project(SAMobile VERSION ${SAMOBILE_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS On)

set(SRC_DIR ${SAMobile_SOURCE_DIR})
add_library(saclient MODULE)

# https://developer.android.com/ndk/guides/cpp-support#libc
# RTTI is enable by default for ensure a better and safety objects handle
target_compile_options(saclient PRIVATE -Wall -Werror -frtti
    $<$<CONFIG:Release>:-Ofast;-fno-stack-protector;-DNDEBUG>)
target_link_options(saclient PRIVATE $<$<CONFIG:Release>:-s;-Wl,--gc-sections>)

target_sources(saclient PRIVATE
    linux_hierarchy.cpp
    log_client.cpp
    patches_level.cpp
    menu_handler.cpp
    plugin_jni.cpp
    nv_threads.cpp
    texture_runtime.cpp
    main_thread.cpp
    jvm_helper.cpp

    render/engine.cpp
    
    game/game_objects.cpp
    game/clock.cpp
    game/render.cpp

    ui/user_graphics.cpp
    ui/imgui_rw.cpp)

add_library(vendor_imgui STATIC
    ${SRC_DIR}/vendor/imgui/imgui_draw.cpp
    ${SRC_DIR}/vendor/imgui/imgui.cpp
    # $<$<CONFIG:DEBUG>:${SRC_DIR}/vendor/imgui/imgui_demo.cpp>
    )

target_include_directories(vendor_imgui INTERFACE
    # Does #include <imgui/imgui.h> be available for us
    $<BUILD_INTERFACE:${SRC_DIR}/vendor>
    $<INSTALL_INTERFACE:imgui>)

execute_process(COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

set(SAMOBILE_PROJECTVERSION ${saclient_PROJECT_VERSION}.${GIT_HASH})
add_subdirectory(vendor/fmt EXCLUDE_FROM_ALL)

target_link_libraries(saclient vendor_imgui log nativehelper $<$<CONFIG:Debug>:fmt::fmt>)
set_target_properties(saclient PROPERTIES OUTPUT_NAME ${SAMOBILE_SHARED_NAME})
target_include_directories(saclient PRIVATE ${SRC_DIR})

install(TARGETS saclient LIBRARY DESTINATION ${SAMobile_SOURCE_DIR}/${SAMOBILE_OUTRELDIR})
