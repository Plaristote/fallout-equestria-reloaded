set(QML_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qml/com/planed")

file(GLOB_RECURSE FOENGINE_QML_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    "${QML_GAME_DIR}/foengine/*.qml")

file(GLOB_RECURSE FOENGINE_JS_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    "${QML_GAME_DIR}/foengine/*.mjs")

file(GLOB_RECURSE FOEDITOR_QML_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    "${QML_GAME_DIR}/foeditor/*.qml")

file(GLOB_RECURSE EDITOR_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/editor/*.cpp")
file(GLOB_RECURSE TILEMAP_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/tilemap/*.cpp")
file(GLOB_RECURSE CMAP_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cmap/*.cpp")
file(GLOB_RECURSE UTILS_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp")
file(GLOB_RECURSE GAME_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp")

set(PROJECT_SOURCES
    ${TILEMAP_FILES} ${CMAP_FILES} ${UTILS_FILES} ${GAME_FILES}
    main.cpp
    credits.h
    credits.cpp
    gamepackages.h
    gamepackages.cpp
    gamecontext.h
    gamecontext.cpp
    gamemanager.h
    gamemanager.cpp
    musicmanager.h
    musicmanager.cpp
    i18n.h
    i18n.cpp
    game.h
    game.cpp
    qml.qrc
)

if (GAME_EDITOR)
  add_compile_definitions(-DGAME_EDITOR)
  set(PROJECT_SOURCES ${PROJECT_SOURCES}
      ${EDITOR_FILES}
      editor.qrc
  )
endif()

if (WIN32)
  qt_add_executable(falloutequestria MANUAL_FINALIZATION WIN32 ${PROJECT_SOURCES})
else()
  qt_add_executable(falloutequestria MANUAL_FINALIZATION ${PROJECT_SOURCES})
endif()

# I would LOVE to move to the proper way of building shaders for Qt, but it just does
# not work (produces broken QSB, apparently). Meanwhile, my crappy build-shaders.sh
# DOES work. So fuck the following:

#qt_add_shaders(falloutequestria "falloutequestria_renderer_shaders"
#  PREFIX "/game/level/renderer"
#  BASE "game/level/renderer"
#  FILES game/level/renderer/tile.shader.vert game/level/renderer/tile.shader.frag
#)

#qt_add_shaders(falloutequestria "falloutequestria_hud_shaders"
#  PREFIX "/game/hub"
#  BASE "game/hub"
#  FILES game/hud/InteractionColorOverlay.shader.vert game/hud/InteractionColorOverlay.shader.frag
#)

qt_add_qml_module(falloutequestria_qml
  URI com.planed.foengine
  VERSION 1.0
  OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/com/planed/foengine"
  QML_FILES ${FOENGINE_QML_FILES}
  RESOURCES ${FOENGINE_JS_FILES}
)

if (GAME_EDITOR)
  qt_add_qml_module(falloutequestria_qmleditor
  URI com.planed.foeditor
  VERSION 1.0
  OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/com/planed/foeditor"
  QML_FILES ${FOEDITOR_QML_FILES}
  RESOURCES ${FOEDITOR_JS_FILES}
)
endif()

set(ENGINE_LINK_LIBRARIES 
          Qt${QT_VERSION_MAJOR}::Core
          Qt${QT_VERSION_MAJOR}::Quick
          Qt${QT_VERSION_MAJOR}::QuickControls2
          Qt${QT_VERSION_MAJOR}::Gui
          Qt${QT_VERSION_MAJOR}::Multimedia)

if (TARGET Qt${QT_VERSION_MAJOR}::Gamepad)
  set(ENGINE_LINK_LIBRARIES ${ENGINE_LINK_LIBRARIES}
          Qt${QT_VERSION_MAJOR}::Gamepad)
  add_definitions(-DWITH_GAMEPAD)
endif()

if (TARGET Qt${QT_VERSION_MAJOR}::GamepadLegacy)
  set(ENGINE_LINK_LIBRARIES ${ENGINE_LINK_LIBRARIES}
          Qt${QT_VERSION_MAJOR}::GamepadLegacy)
  add_definitions(-DWITH_GAMEPAD)
endif()

set(ENGINE_LINK_LIBRARIES ${ENGINE_LINK_LIBRARIES} falloutequestria_qml)
if (GAME_EDITOR)
  set(ENGINE_LINK_LIBRARIES ${ENGINE_LINK_LIBRARIES} falloutequestria_qmleditor)
endif()

target_compile_definitions(falloutequestria
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(falloutequestria
  PRIVATE ${ENGINE_LINK_LIBRARIES}
)

qt_import_qml_plugins(falloutequestria)
qt_finalize_executable(falloutequestria)

install(TARGETS falloutequestria DESTINATION ${CMAKE_INSTALL_BINDIR})
