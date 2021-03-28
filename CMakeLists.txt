cmake_minimum_required(VERSION 3.14)

project(falloutequestria LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package(...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick LinguistTools Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick LinguistTools Gui REQUIRED)
find_package(Qt5Multimedia REQUIRED)

set(TS_FILES falloutequestria_en_US.ts)
set(QT_USE_QTMULTIMEDIA TRUE)

set(PROJECT_SOURCES
        main.cpp
        utils/storableobject.h
        utils/storableobject.cpp
        utils/datetime.hpp
        utils/datetime.cpp
        tilemap/tileset.h
        tilemap/tileset.cpp
        tilemap/tile.h
        tilemap/tile.cpp
        tilemap/tilelayer.h
        tilemap/tilelayer.cpp
        tilemap/tilezone.h
        tilemap/tilezone.cpp
        tilemap/tilemap.h
        tilemap/tilemap.cpp
        cmap/statmodel.h
        cmap/statmodel.cpp
        cmap/plugin.h
        cmap/plugin.cpp
        cmap/trait.h
        cmap/trait.cpp
        cmap/race.h
        cmap/race.cpp
        game/scriptcontroller.h
        game/scriptcontroller.cpp
        game/dataengine.h
        game/dataengine.cpp
        game/level/levelbase.cpp
        game/level/levelbase.h
        game/leveltask.h
        game/leveltask.cpp
        game/levelgrid.h
        game/levelgrid.cpp
        game/dynamicobject.h
        game/dynamicobject.cpp
        game/character.h
        game/character.cpp
        game/characterparty.h
        game/characterparty.cpp
        game/characterdialog.h
        game/characterdialog.cpp
        game/characters/sight.h
        game/characters/sight.cpp
        game/characters/inventory.h
        game/characters/inventory.cpp
        game/characters/statistics.h
        game/characters/statistics.cpp
        game/characters/diplomacy.h
        game/characters/diplomacy.cpp
        game/characters/field_of_view.hpp
        game/characters/field_of_view.cpp
        game/characters/movement.h
        game/characters/movement.cpp
        game/characters/actionqueue.h
        game/characters/actionqueue.cpp
        game/characters/actions/base.h
        game/characters/actions/movement.h
        game/characters/actions/movement.cpp
        game/characters/actions/interaction.h
        game/characters/actions/interaction.cpp
        game/characters/actions/itemUse.h
        game/characters/actions/itemUse.cpp
        game/characters/actions/skillUse.h
        game/characters/actions/skillUse.cpp
        game/lootingcontroller.h
        game/lootingcontroller.cpp
        game/sprite.h
        game/sprite.cpp
        game/animationlibrary.h
        game/animationlibrary.cpp
        game/timermanager.h
        game/timermanager.cpp
        game/worldmap/worldmap.h
        game/worldmap/worldmap.cpp
        game/worldmap/worldmapcity.h
        game/worldmap/worldmapcity.cpp
        game/worldmap/worldmapzone.h
        game/worldmap/worldmapzone.cpp
        game/taskrunner.h
        game/taskrunner.cpp
        game/inventory.h
        game/inventory.cpp
        game/inventoryitemlibrary.h
        game/inventoryitemlibrary.cpp
        game/diplomacy.hpp
        game/diplomacy.cpp
        game/soundmanager.h
        game/soundmanager.cpp
        game/questmanager.h
        game/questmanager.cpp
        game/quest.h
        game/quest.cpp
        game/textbubble.h
        game/textbubble.cpp
        game/level/grid.h
        game/level/grid.cpp
        game/level/interaction.h
        game/level/interaction.cpp
        game/level/combat.h
        game/level/combat.cpp
        game/level/textbubbles.h
        game/level/textbubbles.cpp
        game/level/visualeffects.h
        game/level/visualeffects.cpp
        game/level/playervisibility.h
        game/level/playervisibility.cpp
        game/level/prerender.h
        game/level/prerender.cpp
        game/level/zone.h
        game/level/zone.cpp
        game/objects/doorway.h
        game/objects/doorway.cpp
        game/objects/storageobject.h
        game/objects/storageobject.cpp
        game/objects/inventoryitem.h
        game/objects/inventoryitem.cpp
        game/animationSequence/ianimationpart.h
        game/animationSequence/objectanimationpart.h
        game/animationSequence/objectanimationpart.cpp
        game/animationSequence/spriteanimationpart.h
        game/animationSequence/spriteanimationpart.cpp
        game/animationSequence/animationsequence.h
        game/animationSequence/animationsequence.cpp
        editor/scripteditorcontroller.h
        editor/scripteditorcontroller.cpp
        editor/characterdialogeditor.h
        editor/characterdialogeditor.cpp
        editor/leveleditorcontroller.h
        editor/leveleditorcontroller.cpp
        gamemanager.h
        gamemanager.cpp
        musicmanager.h
        musicmanager.cpp
        i18n.h
        i18n.cpp
        game.h
        game.cpp
        qml.qrc
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(falloutequestria
        ${PROJECT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(falloutequestria SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(falloutequestria
          ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_compile_definitions(falloutequestria
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(falloutequestria
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
          Qt${QT_VERSION_MAJOR}::Quick
          Qt${QT_VERSION_MAJOR}::Gui
          Qt${QT_VERSION_MAJOR}::Multimedia
)
