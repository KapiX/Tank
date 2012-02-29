# Locate SDL_mixer library
# This module defines
# SDLMIXER_LIBRARY, the name of the library to link against
# SDLMIXER_FOUND, if false, do not try to link to SDL
# SDLMIXER_INCLUDE_DIR, where to find SDL/SDL.h
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

FIND_PATH(SDLMIXER_INCLUDE_DIR SDL_mixer.h
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

FIND_LIBRARY(SDLMIXER_LIBRARY 
  NAMES SDL_mixer
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

SET(SDLMIXER_FOUND "NO")
IF(SDLMIXER_LIBRARY AND SDLMIXER_INCLUDE_DIR)
  SET(SDLMIXER_FOUND "YES")
ENDIF(SDLMIXER_LIBRARY AND SDLMIXER_INCLUDE_DIR)

IF(SDL_mixer_FIND_REQUIRED)
  IF(NOT SDLMIXER_FOUND)
    MESSAGE(FATAL_ERROR "SDL_mixer not found")
  ENDIF(NOT SDLMIXER_FOUND)
ENDIF(SDL_mixer_FIND_REQUIRED)
