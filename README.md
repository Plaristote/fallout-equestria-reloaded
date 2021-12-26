# fallout equestrian engine

This repository contains a RPG engine, using a 2D isometric view, and mechanics inspired from the early Fallout games.

The engine is designed to be heavily moddable and relies entirely on the Qt framework, JavaScript and QML.

## Game design

The engine will need game assets to run. You can find a boilerplate to start a new game project at [falloutequestria-boilerplate](https://github.com/Plaristote/falloutequestria-boilerplate).

The falloutequestria-boilerplate repository also features a `demo` branch, containing a technical demo.

You may then download a build of the engine (or build it yourself), launch it from your project's directory, and start editing it using the game editor. More details
on this topic are available in the [Game Editor tutorial](https://plaristote.github.io/fallout-equestria-reloaded/index.html).

## Building on Windows
- Install QtCreator and Qt5.15 by downloading the Qt installer at [Qt.io](https://www.qt.io/download-open-source).
- Open `CMakeLists.txt` with QtCreator
- Set the running directory to your game's asset directory (see `boilerplate`).

## Building on Linux
You will need to install Qt 5.15 before building.

```
  mkdir build
  cd build
  cmake ..
  make
```

Make sure to launch the executable from your game's asset directory:
```
  cd falloutequestria-boilerplate
  ../build/falloutequestria
```
