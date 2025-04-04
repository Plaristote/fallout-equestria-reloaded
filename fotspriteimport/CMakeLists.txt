set(FOT_SPRITE_IMPORT_SOURCES
  spriteImport.cpp
)

qt_add_executable(fotspriteimport ${FOT_SPRITE_IMPORT_SOURCES})

target_link_libraries(fotspriteimport
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
          Qt${QT_VERSION_MAJOR}::Multimedia
)
