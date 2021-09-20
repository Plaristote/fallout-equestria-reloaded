#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "globals.h"
#include "i18n.h"
#include "game/animationlibrary.h"
#include "game/inventoryitemlibrary.h"

#include "tilemap/tilemap.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tilezone.h"
#include "tilemap/tile.h"

#include "game.h"
#include "game/dices.hpp"
#include "game/dynamicobject.h"
#include "game/objects/inventoryitem.h"
#include "game/objects/doorway.h"
#include "game/leveltask.h"
#include "game/characterdialog.h"
#include "game/lootingcontroller.h"
#include "gamemanager.h"
#include "musicmanager.h"
#include "game/characters/buff.h"
#include "game/characters/actionqueue.h"
#include "game/mousecursor.h"
#include "game/gamepadcontroller.h"

#include "cmap/statmodel.h"

#include "editor/scripteditorcontroller.h"
#include "editor/characterdialogeditor.h"
#include "editor/leveleditorcontroller.h"
#include "editor/gameobjecttemplates.h"

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

  QCoreApplication::setOrganizationName("Ile Noire Dev");
  QCoreApplication::setOrganizationDomain("ile-noire.fr");
  QCoreApplication::setApplicationName("Fallout Equestria RPG");
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
  MouseCursor* cursor = new MouseCursor(&app);
  GamepadController* gamepad = new GamepadController(&app);

  app.setWindowIcon(QIcon(":/assets/icon.ico"));
  cursor->updatePointerType();

  qmlRegisterType<I18n>("I18n", 1,0, "I18n");
  qmlRegisterType<Game>("Game", 1,0, "Controller");
  qmlRegisterType<StatModel>("Game", 1,0, "StatModel");
  qmlRegisterType<Sprite>("Game", 1,0, "Sprite");
  qmlRegisterType<DynamicObject>("Game", 1,0, "DynamicObject");
  qmlRegisterType<Character>("Game", 1,0, "Character");
  qmlRegisterType<StorageObject>("Game", 1,0, "StorageObject");
  qmlRegisterType<Doorway>("Game", 1,0, "Doorway");
  qmlRegisterType<InventoryItem>("Game", 1,0, "InventoryItem");
  qmlRegisterType<Inventory>("Game", 1,0, "Inventory");
  qmlRegisterType<CharacterDialog>("Game", 1,0, "CharacterDialog");
  qmlRegisterType<LootingController>("Game", 1,0, "LootingController");
  qmlRegisterType<GameManager>("Game", 1,0, "GameManager");
  qmlRegisterType<MusicManager>("Game", 1,0, "MusicManager");
  qmlRegisterType<QuestManager>("Game", 1,0, "QuestManager");
  qmlRegisterType<Quest>("Game", 1,0, "Quest");
  qmlRegisterType<WorldMap>("Game", 1,0, "WorldMap");
  qmlRegisterType<WorldMapCity>("Game", 1,0, "WorldMapCity");
  qmlRegisterType<WorldMapZone>("Game", 1,0, "WorldMapZone");
  qmlRegisterType<Inventory>("Game", 1,0, "Inventory");
  qmlRegisterType<TextBubble>("Game", 1,0, "TextBubble");
  qmlRegisterType<Buff>("Game", 1,0, "Buff");
  qmlRegisterType<TutorialComponent>("Game", 1,0, "TutorialComponent");
  qmlRegisterType<ActionQueue>("Game", 1,0, "ActionQueue");
  qmlRegisterType<InteractionTargetList>("Game", 1,0, "InteractionTargetList");

  qRegisterMetaType<Character*>("const Character*");
  qRegisterMetaType<CharacterDiplomacy*>("const CharacterDiplomacy*");
  qRegisterMetaType<DynamicObject*>("const DynamicObject*");

  registerQmlTilemap();
  // GAME EDITOR
  qmlRegisterType<QmlSpriteAnimation>("Game", 1,0, "SpriteAnimation");
  qmlRegisterType<CharacterDialogEditor>("Game", 1,0, "CharacterDialogEditor");
  qmlRegisterType<LevelEditorController>("Game", 1,0, "LevelEditorController");
  qmlRegisterType<GameObjectTemplates>("Game", 1,0, "GameObjectTemplates");
  GameObjectTemplates* got = new GameObjectTemplates(&app);
  got->initialize();
  engine.rootContext()->setContextProperty("gameObjectTemplates", got);
  // END GAME EDITOR

  MusicManager* musicManager = new MusicManager(&app);
  GameManager*  gameManager = new GameManager();
  I18n*         i18n = new I18n(&app);

  engine.rootContext()->setContextProperty("i18n", i18n);
  engine.rootContext()->setContextProperty("gameManager", gameManager);
  engine.rootContext()->setContextProperty("musicManager", musicManager);
  engine.rootContext()->setContextProperty("animationLibrary", &animationLibrary);
  engine.rootContext()->setContextProperty("itemLibrary", &itemLibrary);
  engine.rootContext()->setContextProperty("fileProtocol", fileProtocol);
  engine.rootContext()->setContextProperty("rootPath", rootPath);
  engine.rootContext()->setContextProperty("scriptPath", scriptPath);
  engine.rootContext()->setContextProperty("assetPath", assetPath);
  engine.rootContext()->setContextProperty("scriptController", &scriptEditorController);
  engine.rootContext()->setContextProperty("gamepad", gamepad);
  engine.rootContext()->setContextProperty("mouseCursor", cursor);

  QObject::connect(i18n, &I18n::currentLocaleChanged, &app, [&engine, i18n]()
  {
    engine.rootContext()->setContextProperty("i18n", i18n);
  });

  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url, cursor](QObject *obj, const QUrl &objUrl)
  {
    if (!obj && url == objUrl)
      QCoreApplication::exit(-1);
    else
      cursor->setWindow(obj);
  }, Qt::QueuedConnection);
  engine.load(url);
  return app.exec();
}
