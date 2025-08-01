cmake_minimum_required(VERSION 3.14)

project(falloutequestria LANGUAGES CXX)

option(GAME_EDITOR "development edition" ON)

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

if (WIN32)
  execute_process(COMMAND build-shaders.bat WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
else ()
  execute_process(COMMAND ./build-shaders.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

find_package(QT NAMES Qt6 COMPONENTS Core Quick Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick QuickControls2 Gui)
find_package(Qt${QT_VERSION_MAJOR}Multimedia REQUIRED)

if (WIN32) # Workaround broken QtGamepadLegacy build on Windows
  find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED COMPONENTS BuildInternals Core Gui)
  find_package(Qt${QT_VERSION_MAJOR} CONFIG OPTIONAL_COMPONENTS Quick QuickControls2)
endif()

set(QT_USE_QTMULTIMEDIA TRUE)

add_subdirectory(qtgamepadlegacy)
add_subdirectory(src)
add_subdirectory(fotspriteimport)
