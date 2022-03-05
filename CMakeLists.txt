cmake_minimum_required (VERSION 3.8)

project(client)

set(CMAKE_C_FLAGS_RELEASE "-O3 -fvisibility=hidden -flto -DNDEBUG -Wfatal-errors") # -fno-exceptions

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-rtti")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} --no-gnu-unique")
endif()

add_compile_definitions (
	SUBHOOK_SEPARATE_SOURCE_FILES
	SUBHOOK_IMPLEMENTATION
)

# Allow threading
set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

find_package(Freetype REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_library (client SHARED
	../subhook/subhook.c
	../subhook/subhook_unix.c
	../subhook/subhook_x86.c
	../imgui/imgui_draw.cpp
	../imgui/imgui_demo.cpp
	../imgui/imgui_tables.cpp
	../imgui/imgui_widgets.cpp
	../imgui/imgui.cpp
	../imgui/misc/freetype/imgui_freetype.cpp
	../imgui/backends/imgui_impl_opengl3.cpp
	../imgui/backends/imgui_impl_sdl.cpp
	src/networking/tcpSocket.cpp
	src/hooks.cpp
	src/game.cpp
	src/main.cpp
	src/api.cpp
	src/gui/console.cpp
	src/events.cpp
	src/utils/settings.cpp
	src/networking/serverConnection.cpp
	src/drawText.asm
)

if (WIN32)
	find_library(WSOCK32_LIBRARY wsock32)
	find_library(WS2_32_LIBRARY ws2_32)
	target_link_libraries(client wsock32 ws2_32)
endif()

set_property (TARGET client PROPERTY CXX_STANDARD 20)

target_include_directories(client PRIVATE ${SDL2_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(client PRIVATE SDL2 GL ${CMAKE_DL_LIBS} Threads::Threads stdc++fs Freetype::Freetype)
include_directories (${CMAKE_SOURCE_DIR}/imgui)
include_directories (${CMAKE_SOURCE_DIR}/imgui/backends)
include_directories (${CMAKE_SOURCE_DIR}/subhook)
include_directories (${CMAKE_SOURCE_DIR}/ImTerm/include)
include_directories (${CMAKE_SOURCE_DIR}/spdlog/include)
target_link_options(client PRIVATE LINKER:--no-undefined)
