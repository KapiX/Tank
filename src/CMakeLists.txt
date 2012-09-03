add_subdirectory(7z)

list(APPEND SRC
  ${7Z_SOURCES}
  7ZipArchive.cpp
  Animation.cpp
  Bonus.cpp
  Config.cpp
  Directory.cpp
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

if(WITH_OPENGL)
  list(APPEND SRC
    OGLRenderList.cpp
    OGLTexture.cpp
    OGLVideoDriver.cpp
  )
endif(WITH_OPENGL)
if(WITH_D3D8)
  list(APPEND SRC
    D3D8RenderList.cpp
    D3D8Texture.cpp
    D3D8VideoDriver.cpp
  )
endif(WITH_D3D8)

foreach(F ${SRC})
  list(APPEND TANK_SOURCES src/${F})
endforeach(F)

set(TANK_SOURCES ${TANK_SOURCES} PARENT_SCOPE)