add_subdirectory(7z)

list(APPEND SRC
  ${7Z_SOURCES}
  7ZipArchive.cpp
  Animation.cpp
  Bonus.cpp
  Config.cpp
  Enemy.cpp
  Game.cpp
  LevelPack.cpp
  main.cpp
  Map.cpp
  Menu.cpp
  Player.cpp
  SoundManager.cpp
  Tank.cpp
  Timer.cpp
  Window.cpp
)

if(WIN32)
  list(APPEND SRC
    Directory_win32.cpp
  )
elseif(UNIX OR BEOS)
  list(APPEND SRC
    Directory_unix.cpp
  )
endif(WIN32)

if(WITH_OPENGL)
  list(APPEND SRC
    OGLRenderList.cpp
    OGLTexture.cpp
    OGLVideoDriver.cpp
  )
endif(WITH_OPENGL)

if(HAIKU)
  list(APPEND SRC
    Tank.rdef
  )
endif(HAIKU)

foreach(F ${SRC})
  list(APPEND TANK_SOURCES src/${F})
endforeach(F)

set(TANK_SOURCES ${TANK_SOURCES} PARENT_SCOPE)