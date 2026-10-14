cmake_minimum_required(VERSION 3.16)
include(FetchContent)

project(planets
  VERSION 0.1.0
  DESCRIPTION "Work in progress game about planets"
  LANGUAGES C
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# QOL update for DOOM emacs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Raylib and Box2D as github repos
#find_package(raylib REQUIRED)
#find_package(box2d REQUIRED)
FetchContent_Declare(
	raylib
	GIT_REPOSITORY git@github.com:raysan5/raylib.git
	GIT_TAG master
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
	box2d
	GIT_REPOSITORY git@github.com:erincatto/box3d.git
	GIT_TAG main
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
set(BOX2D_UNIT_TESTS OFF CACHE INTERNAL "")
set(BOX2D_SAMPLES OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(box2d)

# Make executable
add_executable(${PROJECT_NAME} main.c)

# Add the images in the assets directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets"
	DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
)

# Link raylib and box2d
target_link_libraries(${PROJECT_NAME} PRIVATE raylib box2d)
