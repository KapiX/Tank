list(APPEND SRC
  7zAlloc.c
  7zBuf.c
  7zBuf2.c
  7zCrc.c
  7zCrcOpt.c
  7zDec.c
  7zFile.c
  7zIn.c
  7zStream.c
  Bcj2.c
  Bra.c
  Bra86.c
  CpuArch.c
  Lzma2Dec.c
  LzmaDec.c
  Ppmd7.c
  Ppmd7Dec.c
)

foreach(F ${SRC})
  list(APPEND 7Z_SOURCES 7z/${F})
endforeach(F)

set(7Z_SOURCES ${7Z_SOURCES} PARENT_SCOPE)

add_definitions(-D_7ZIP_ST)