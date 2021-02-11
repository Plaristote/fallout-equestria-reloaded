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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick LinguistTools REQUIRED)

set(TS_FILES falloutequestria_en_US.ts)

set(PROJECT_SOURCES
        main.cpp
        tilemap/tileset.h
        tilemap/tileset.cpp
        tilemap/tile.h
        tilemap/tile.cpp
        tilemap/tilelayer.h
        tilemap/tilelayer.cpp
        tilemap/tilezone.h
        tilemap/tilezone.cpp
        tilemap/objecttileset.h
        tilemap/objecttileset.cpp
        tilemap/tilemap.h
        tilemap/tilemap.cpp
        cmap/statmodel.h
        cmap/statmodel.cpp
        cmap/trait.h
        cmap/trait.cpp
        game/dataengine.h
        game/dataengine.cpp
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
        game/sprite.h
        game/sprite.cpp
        game/animationlibrary.h
        game/animationlibrary.cpp
        game/datetime.hpp
        game/datetime.cpp
        game/timermanager.h
        game/timermanager.cpp
        game/worldmap.h
        game/worldmap.cpp
        game/worldmapcity.h
        game/worldmapcity.cpp
        gamemanager.h
        gamemanager.cpp
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
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)
