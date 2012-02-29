project(Tank)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(GLEW REQUIRED)

include_directories(include ${SDL_INCLUDE_DIR} ${GLEW_INCLUDE_PATH})
add_subdirectory(src)

option(WITH_OPENGL "Enable OpenGL renderer" ON)
if(WIN32)
  option(WITH_D3D8 "Enable Direct3D renderer" OFF)
endif(WIN32)
  
configure_file(${CMAKE_SOURCE_DIR}/include/buildconfig.h.cmake ${CMAKE_SOURCE_DIR}/include/buildconfig.h)

add_executable(tank WIN32 ${TANK_SOURCES})
target_link_libraries(tank ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${GLEW_LIBRARY})

add_custom_command(
  TARGET tank
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/data/graphics"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/graphics"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/data/levels"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/levels"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/data/sounds"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sounds"
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/data/icon.bmp"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/icon.bmp"
)