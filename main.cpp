#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "globals.h"
#include "game/animationlibrary.h"
#include "game/inventoryitemlibrary.h"

#include "tilemap/tilemap.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tilezone.h"
#include "tilemap/tile.h"

#include "game.h"
#include "game/dices.hpp"
#include "game/dynamicobject.h"
#include "game/inventoryitem.h"
#include "game/leveltask.h"
#include "game/characterdialog.h"
#include "game/lootingcontroller.h"
#include "gamemanager.h"
#include "musicmanager.h"

#include "cmap/statmodel.h"

#include "editor/scripteditorcontroller.h"

void registerQmlTilemap() {
  qmlRegisterType<TileMap>  ("Tiles", 1,0, "TileMap");
  qmlRegisterType<TileLayer>("Tiles", 1,0, "TileLayer");
  qmlRegisterType<Tile>     ("Tiles", 1,0, "Tile");
  qmlRegisterType<TileZone> ("Tiles", 1,0, "TileZone");
}

int main(int argc, char *argv[])
{
  auto fileProtocol = FILE_PROTOCOL;
  auto rootPath     = FILE_PROTOCOL + ROOT_PATH;
  auto scriptPath   = FILE_PROTOCOL + SCRIPTS_PATH;
  auto assetPath    = FILE_PROTOCOL + ASSETS_PATH;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  Dices::Initialize();

  AnimationLibrary animationLibrary;
  animationLibrary.initialize();

  InventoryItemLibrary itemLibrary;
  itemLibrary.initialize();

  ScriptEditorController scriptEditorController;

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  qmlRegisterType<Game>("Game", 1,0, "Controller");
  qmlRegisterType<StatModel>("Game", 1,0, "StatModel");
  qmlRegisterType<DynamicObject>("Game", 1,0, "DynamicObject");
  qmlRegisterType<Character>("Game", 1,0, "Character");
  qmlRegisterType<StorageObject>("Game", 1,0, "StorageObject");
  qmlRegisterType<InventoryItem>("Game", 1,0, "InventoryItem");
  qmlRegisterType<Inventory>("Game", 1,0, "Inventory");
  qmlRegisterType<CharacterDialog>("Game", 1,0, "CharacterDialog");
  qmlRegisterType<LootingController>("Game", 1,0, "LootingController");
  qmlRegisterType<GameManager>("Game", 1,0, "GameManager");
  qmlRegisterType<MusicManager>("Game", 1,0, "MusicManager");
  qmlRegisterType<WorldMap>("Game", 1,0, "WorldMap");
  qmlRegisterType<WorldMapCity>("Game", 1,0, "WorldMapCity");
  qmlRegisterType<Inventory>("Game", 1,0, "Inventory");
  qmlRegisterType<TextBubble>("Game", 1,0, "TextBubble");

  registerQmlTilemap();
  // GAME EDITOR
  qmlRegisterType<QmlSpriteAnimation>("Game", 1,0, "SpriteAnimation");
  // END GAME EDITOR

  MusicManager* musicManager = new MusicManager(&app);
  GameManager*  gameManager = new GameManager();

  engine.rootContext()->setContextProperty("gameManager", gameManager);
  engine.rootContext()->setContextProperty("musicManager", musicManager);
  engine.rootContext()->setContextProperty("animationLibrary", &animationLibrary);
  engine.rootContext()->setContextProperty("itemLibrary", &itemLibrary);
  engine.rootContext()->setContextProperty("fileProtocol", fileProtocol);
  engine.rootContext()->setContextProperty("rootPath", rootPath);
  engine.rootContext()->setContextProperty("scriptPath", scriptPath);
  engine.rootContext()->setContextProperty("assetPath", assetPath);
  engine.rootContext()->setContextProperty("scriptController", &scriptEditorController);

  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl)
  {
    if (!obj && url == objUrl)
      QCoreApplication::exit(-1);
  }, Qt::QueuedConnection);
  engine.load(url);
  return app.exec();
}
