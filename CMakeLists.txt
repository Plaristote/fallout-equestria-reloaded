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

find_package(QT NAMES Qt5 COMPONENTS Core Quick LinguistTools Gui Gamepad REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick LinguistTools Gui Gamepad REQUIRED)
find_package(Qt5Multimedia REQUIRED)

set(QT_USE_QTMULTIMEDIA TRUE)

set(PROJECT_SOURCES
        main.cpp
        credits.h
        credits.cpp
        utils/storableobject.h
        utils/storableobject.cpp
        utils/datetime.hpp
        utils/datetime.cpp
        utils/layeredspritesheet.h
        utils/layeredspritesheet.cpp
        tilemap/tileset.h
        tilemap/tileset.cpp
        tilemap/tile.h
        tilemap/tile.cpp
        tilemap/tilelayer.h
        tilemap/tilelayer.cpp
        tilemap/tilezone.h
        tilemap/tilezone.cpp
        tilemap/floorlayer.h
        tilemap/floorlayer.cpp
        tilemap/tilemap.h
        tilemap/tilemap.cpp
        tilemap/properties.cpp
        tilemap/painter.cpp
        cmap/statmodel.h
        cmap/statmodel.cpp
        cmap/plugin.h
        cmap/plugin.cpp
        cmap/trait.h
        cmap/trait.cpp
        cmap/race.h
        cmap/race.cpp
        cmap/perk.h
        cmap/perk.cpp
        game/savepreview.h
        game/savepreview.cpp
        game/mousecursor.h
        game/mousecursor.cpp
        game/gamepadcontroller.h
        game/gamepadcontroller.cpp
        game/scriptcontroller.h
        game/scriptcontroller.cpp
        game/dataengine.h
        game/dataengine.cpp
        game/level/levelbase.cpp
        game/level/levelbase.h
        game/level/metrics.h
        game/level/metrics.cpp
        game/level/clock.h
        game/level/clock.cpp
        game/level/debug.h
        game/level/debug.cpp
        game/level/save.h
        game/level/save.cpp
        game/leveltask.h
        game/leveltask.cpp
        game/dynamicobject.h
        game/dynamicobject.cpp
        game/character.h
        game/character.cpp
        game/characterparty.h
        game/characterparty.cpp
        game/characterdialog.h
        game/characterdialog.cpp
        game/bartercontroller.h
        game/bartercontroller.cpp
        game/characters/buff.h
        game/characters/buff.cpp
        game/characters/buffs.h
        game/characters/buffs.cpp
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
        game/characters/actions/reach.h
        game/characters/actions/reach.cpp
        game/characters/actions/reachcase.h
        game/characters/actions/reachcase.cpp
        game/characters/actions/reachdoor.h
        game/characters/actions/reachdoor.cpp
        game/characters/actions/sliding.h
        game/characters/actions/sliding.cpp
        game/characters/actions/waitaction.h
        game/characters/actions/waitaction.cpp
        game/characters/actions/speak.h
        game/characters/actions/speak.cpp
        game/characters/actions/look.h
        game/characters/actions/look.cpp
        game/characters/actions/script.h
        game/characters/actions/script.cpp
        game/lootingcontroller.h
        game/lootingcontroller.cpp
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
        game/worldmap/randomencountercontroller.h
        game/worldmap/randomencountercontroller.cpp
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
        game/timepasser.h
        game/timepasser.cpp
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
        game/level/tutorialcomponent.cpp
        game/level/interactiontargetlist.h
        game/level/interactiontargetlist.cpp
        game/level/lineofsight.cpp
        game/level/ambientlightcomponent.h
        game/level/ambientlightcomponent.cpp
        game/objects/objectgroup.h
        game/objects/objectgroup.cpp
        game/objects/objectfactory.h
        game/objects/objectfactory.cpp
        game/objects/doorway.h
        game/objects/doorway.cpp
        game/objects/storageobject.h
        game/objects/storageobject.cpp
        game/objects/inventoryitem.h
        game/objects/inventoryitem.cpp
        game/objects/bloodstain.h
        game/objects/bloodstain.cpp
        game/objects/elevator.h
        game/objects/elevator.cpp
        game/objects/components/scriptable.h
        game/objects/components/scriptable.cpp
        game/objects/components/gridobjectcomponent.h
        game/objects/components/gridobjectcomponent.cpp
        game/objects/components/sprite.h
        game/objects/components/sprite.cpp
        game/objects/components/orientedsprite.h
        game/objects/components/orientedsprite.cpp
        game/objects/components/controlzone.h
        game/objects/components/controlzone.cpp
        game/objects/components/detectable.h
        game/objects/components/detectable.cpp
        game/objects/components/lightsource.h
        game/objects/components/lightsource.cpp
        game/animationSequence/ianimationpart.h
        game/animationSequence/ianimationpart.cpp
        game/animationSequence/objectanimationpart.h
        game/animationSequence/objectanimationpart.cpp
        game/animationSequence/spriteanimationpart.h
        game/animationSequence/spriteanimationpart.cpp
        game/animationSequence/animationsequence.h
        game/animationSequence/animationsequence.cpp
        game/animationSequence/movementhintanimationpart.h
        game/animationSequence/movementhintanimationpart.cpp
        game/animationSequence/groupanimationpart.h
        game/animationSequence/groupanimationpart.cpp
        game/animationSequence/animationloader.h
        game/pathfinding/caselocker.h
        game/pathfinding/astar.hpp
        game/pathfinding/preparecase.h
        game/pathfinding/preparecase.cpp
        game/pathfinding/levelgrid.h
        game/pathfinding/levelgrid.cpp
        game/pathfinding/levelgrid_prepare.cpp
        game/pathfinding/levelgrid_casecontent.cpp
        game/pathfinding/zonegrid.h
        game/pathfinding/zonegrid.cpp
        game/pathfinding/zonegrid_prepare.cpp
        game/pathfinding/zonegrid_findpath.cpp
        game/pathfinding/pathzone.h
        game/pathfinding/pathzone.cpp
        game/pathfinding/candidatesolution.h
        game/pathfinding/candidatesolution.cpp
        editor/scripteditorcontroller.h
        editor/scripteditorcontroller.cpp
        editor/characterdialogeditor.h
        editor/characterdialogeditor.cpp
        editor/leveleditorcontroller.h
        editor/leveleditorcontroller.cpp
        editor/gameobjecttemplates.h
        editor/gameobjecttemplates.cpp
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

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(falloutequestria ${PROJECT_SOURCES})
else()
  if(ANDROID)
    add_library(falloutequestria SHARED ${PROJECT_SOURCES})
  elseif(WIN32)
    add_executable(falloutequestria WIN32 ${PROJECT_SOURCES})
  else()
    add_executable(falloutequestria ${PROJECT_SOURCES})
  endif()
endif()

target_compile_definitions(falloutequestria
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(falloutequestria
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
          Qt${QT_VERSION_MAJOR}::Quick
          Qt${QT_VERSION_MAJOR}::Gui
          Qt${QT_VERSION_MAJOR}::Multimedia
          Qt${QT_VERSION_MAJOR}::Gamepad
)
