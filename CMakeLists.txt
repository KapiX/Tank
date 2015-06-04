project(Tank)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(WITH_OPENGL "Enable OpenGL renderer" ON)
if(WITH_OPENGL)
  option(WITH_GLEW "Enable GLEW library" ON)
  if(HAIKU)
    set(WITH_GLEW OFF)
  endif(HAIKU)
endif(WITH_OPENGL)

# find packages
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
if(WITH_OPENGL)
  find_package(OpenGL REQUIRED)
  if(WITH_GLEW)
    find_package(GLEW REQUIRED)
  endif(WITH_GLEW)
endif(WITH_OPENGL)

if(HAIKU)
  include(UseHaiku)
endif(HAIKU)

# setup paths and link libraries
list(APPEND INC_PATH
  ${SDL_INCLUDE_DIR}
)
list(APPEND LIBS
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
  ${SDLMIXER_LIBRARY}
)
if(WITH_OPENGL)
  list(APPEND INC_PATH
    ${OPENGL_INCLUDE_DIR}
  )
  list(APPEND LIBS
    ${OPENGL_LIBRARIES}
  )
  if(WITH_GLEW)
    list(APPEND INC_PATH
      ${GLEW_INCLUDE_PATH}
    )
    list(APPEND LIBS
      ${GLEW_LIBRARY}
    )
  endif(WITH_GLEW)
endif(WITH_OPENGL)

include_directories(include ${INC_PATH})
add_subdirectory(src)

configure_file(${CMAKE_SOURCE_DIR}/include/buildconfig.h.cmake ${CMAKE_SOURCE_DIR}/include/buildconfig.h)

# disable deprecation warning
# FIXME
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

source_group(7z REGULAR_EXPRESSION "src/7z/.*")

if(WIN32)
  #string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Disable C++ exceptions 
  string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Disable RTTI
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