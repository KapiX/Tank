# Locate SDL_image library
# This module defines
# SDLIMAGE_LIBRARY, the name of the library to link against
# SDLIMAGE_FOUND, if false, do not try to link to SDL
# SDLIMAGE_INCLUDE_DIR, where to find SDL/SDL.h
#
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
#
# Created by Eric Wing. This was influenced by the FindSDL.cmake 
# module, but with modifications to recognize OS X frameworks and 
# additional Unix paths (FreeBSD, etc).

#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

if(MSVC)
  if(CMAKE_CL_64)
    SET(LIB_SUFFIX "lib64")
  else(CMAKE_CL_64)
    SET(LIB_SUFFIX "lib")
  endif(CMAKE_CL_64)
else(MSVC)
  SET(LIB_SUFFIX "lib64 lib")
endif(MSVC)

FIND_PATH(SDLIMAGE_INCLUDE_DIR SDL_image.h
  HINTS
  $ENV{LIBRARYDIR}
  PATH_SUFFIXES include/SDL include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include/SDL
  /usr/include/SDL
  /usr/local/include/SDL12
  /usr/local/include/SDL11 # FreeBSD ports
  /usr/include/SDL12
  /usr/include/SDL11
  /usr/local/include
  /usr/include
  /sw/include/SDL # Fink
  /sw/include
  /opt/local/include/SDL # DarwinPorts
  /opt/local/include
  /opt/csw/include/SDL # Blastwave
  /opt/csw/include 
  /opt/include/SDL
  /opt/include
)

FIND_LIBRARY(SDLIMAGE_LIBRARY 
  NAMES SDL_image
  HINTS
  $ENV{LIBRARYDIR}
  PATH_SUFFIXES ${LIB_SUFFIX}
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

SET(SDLIMAGE_FOUND "NO")
IF(SDLIMAGE_LIBRARY AND SDLIMAGE_INCLUDE_DIR)
  SET(SDLIMAGE_FOUND "YES")
ENDIF(SDLIMAGE_LIBRARY AND SDLIMAGE_INCLUDE_DIR)

IF(SDL_image_FIND_REQUIRED)
  IF(NOT SDLIMAGE_FOUND)
    MESSAGE(FATAL_ERROR "SDL_image not found")
  ENDIF(NOT SDLIMAGE_FOUND)
ENDIF(SDL_image_FIND_REQUIRED)