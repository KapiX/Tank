project(Tank)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(WITH_OPENGL "Enable OpenGL renderer" ON)
if(WIN32 AND NOT CMAKE_CL_64)
  option(WITH_D3D8 "Enable Direct3D 8 renderer" ON)
endif(WIN32 AND NOT CMAKE_CL_64)

# find packages
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(GLEW REQUIRED)
if(WITH_D3D8)
  find_package(D3D8 REQUIRED)
endif(WITH_D3D8)

# setup paths and link libraries
list(APPEND INC_PATH
  ${SDL_INCLUDE_DIR}
  ${GLEW_INCLUDE_PATH}
)
list(APPEND LIBS
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
  ${SDLMIXER_LIBRARY}
  ${GLEW_LIBRARY}
)
if(WITH_D3D8)
  list(APPEND INC_PATH
    ${D3D8_INCLUDE_PATH}
    ${D3DX8_INCLUDE_PATH}
  )
  list(APPEND LIBS
    ${D3D8_LIBRARY}
    ${D3DX8_LIBRARY}
  )
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libci.lib")
endif(WITH_D3D8)

include_directories(include ${INC_PATH})
add_subdirectory(src)
  
configure_file(${CMAKE_SOURCE_DIR}/include/buildconfig.h.cmake ${CMAKE_SOURCE_DIR}/include/buildconfig.h)

if(WIN32)
  add_executable(tank WIN32 ${TANK_SOURCES} src/resources.rc)
else(WIN32)
  add_executable(tank ${TANK_SOURCES})
endif(WIN32)
target_link_libraries(tank ${LIBS})

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