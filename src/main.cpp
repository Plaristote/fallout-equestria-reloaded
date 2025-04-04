#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "globals.h"
#include "i18n.h"
#include "gamepackages.h"
#include "credits.h"
#include "gamecontext.h"

#include "tilemap/tilemap.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tilezone.h"
#include "tilemap/tile.h"

#include "game.h"
#include "game/dices.hpp"
#include "game/dynamicobject.h"
#include "game/objects/inventoryitem.h"
#include "game/objects/doorway.h"
#include "game/objects/elevator.h"
#include "game/objects/objectfactory.h"
#include "game/leveltask.h"
#include "game/characterdialog.h"
#include "game/lootingcontroller.h"
#include "gamemanager.h"
#include "musicmanager.h"
#include "game/soundmanager.h"
#include "game/characters/buff.h"
#include "game/characters/actionqueue.h"
#include "game/mousecursor.h"
#include "game/gamepadcontroller.h"
#include "game/savepreview.h"
#include "game/diplomacy.hpp"

#include "cmap/statmodel.h"

#include "editor/scripteditorcontroller.h"
#include "editor/characterdialogeditor.h"
#include "editor/leveleditorcontroller.h"
#include "editor/gameobjecttemplates.h"

#include <QResource>

QJSEngine* qmlJsEngine = nullptr;

void registerQmlTilemap()
{
  qmlRegisterType<TileMap>  ("Tiles", 1,0, "TileMap");
  qmlRegisterType<TileLayer>("Tiles", 1,0, "TileLayer");
  qmlRegisterType<Tile>     ("Tiles", 1,0, "Tile");
  qmlRegisterType<TileZone> ("Tiles", 1,0, "TileZone");
}

int main(int argc, char *argv[])
{
  Dices::Initialize();

#ifdef GAME_EDITOR
  QResource::registerResource("editor.rcc");
  ScriptEditorController scriptEditorController;
#endif

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  GamePackages* gamePackages = new GamePackages(&app);
  GameContext* gameContext = nullptr;
  MouseCursor* cursor = new MouseCursor(&app);
  GamepadController* gamepad = new GamepadController(&app);

  app.setWindowIcon(QIcon(":/assets/icon.ico"));
  cursor->updatePointerType();

  qmlRegisterType<I18n>("I18n", 1,0, "I18n");
  qmlRegisterType<Game>("Game", 1,0, "Controller");
  qmlRegisterType<SavePreview>("Game", 1,0, "SavePreview");
  qmlRegisterType<StatModel>("Game", 1,0, "StatModel");
  qmlRegisterType<Sprite>("Game", 1,0, "Sprite");
  qmlRegisterType<DynamicObject>("Game", 1,0, "DynamicObject");
  qmlRegisterType<Character>("Game", 1,0, "Character");
  qmlRegisterType<StorageObject>("Game", 1,0, "StorageObject");
  qmlRegisterType<Doorway>("Game", 1,0, "Doorway");
  qmlRegisterType<Elevator>("Game", 1,0, "Elevator");
  qmlRegisterType<InventoryItem>("Game", 1,0, "InventoryItem");
  qmlRegisterType<Inventory>("Game", 1,0, "Inventory");
  qmlRegisterType<CharacterDialog>("Game", 1,0, "CharacterDialog");
  qmlRegisterType<LootingController>("Game", 1,0, "LootingController");
  qmlRegisterType<GameManager>("Game", 1,0, "GameManager");
  qmlRegisterType<MusicManager>("Game", 1,0, "MusicManager");
  qmlRegisterType<SoundManager>("Game", 1,0, "SoundManager");
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
  qmlRegisterType<LevelGrid>("Game", 1,0, "LevelGrid");
  qmlRegisterType<ObjectGroup>("Game", 1,0, "ObjectGroup");
  qmlRegisterType<Credits>("Game", 1,0, "Credits");
  qmlRegisterType<CreditPerson>("Game", 1,0, "Person");

  qRegisterMetaType<Character*>("Character*");
  qRegisterMetaType<Character*>("const Character*");
  qRegisterMetaType<CharacterDiplomacy*>("const CharacterDiplomacy*");
  qRegisterMetaType<DynamicObject*>("const DynamicObject*");
  qRegisterMetaType<TileZone*>("const TileZone*");
  qRegisterMetaType<ObjectFactory*>("ObjectFactory*");
  qRegisterMetaType<ObjectGroup*>("const ObjectGroup*");
  qRegisterMetaType<GridComponent*>("GridComponent*");
  qRegisterMetaType<WorldDiplomacy*>("WorldDiplomacy*");
  qRegisterMetaType<TileZone*>("const TileZone*");
  qRegisterMetaType<GamePackage*>("GamePackage*");
  qRegisterMetaType<GamePackage*>("const GamePackage*");

  registerQmlTilemap();

#ifdef GAME_EDITOR
  qmlRegisterType<QmlSpriteAnimation>("Game", 1,0, "SpriteAnimation");
  qmlRegisterType<CharacterDialogEditor>("Game", 1,0, "CharacterDialogEditor");
  qmlRegisterType<LevelEditorController>("Game", 1,0, "LevelEditorController");
  qmlRegisterType<GameObjectTemplates>("Game", 1,0, "GameObjectTemplates");
  GameObjectTemplates* got = new GameObjectTemplates(&app);
  got->initialize();
  engine.rootContext()->setContextProperty("gameObjectTemplates", got);
  engine.rootContext()->setContextProperty("scriptController", &scriptEditorController);
#endif

  I18n* i18n = new I18n(&app);

  qmlJsEngine = &engine;
  engine.rootContext()->setContextProperty("i18n", i18n);
  engine.rootContext()->setContextProperty("gamepad", gamepad);
  engine.rootContext()->setContextProperty("mouseCursor", cursor);
  engine.rootContext()->setContextProperty("gamePackages", gamePackages);
#ifdef GAME_EDITOR
  engine.rootContext()->setContextProperty("developmentEdition", true);
#else
  engine.rootContext()->setContextProperty("developmentEdition", false);
#endif

  QObject::connect(i18n, &I18n::currentLocaleChanged, &app, [&engine, i18n]()
  {
    engine.rootContext()->setContextProperty("i18n", i18n);
  });

  const QUrl mainUrl(QStringLiteral("qrc:/main.qml"));
  const QUrl gamePickerUrl(QStringLiteral("qrc:/GamePicker.qml"));

  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [mainUrl, gamePickerUrl, cursor, gamepad](QObject *obj, const QUrl &objUrl)
  {
    if (!obj && (mainUrl == objUrl || gamePickerUrl == objUrl))
      QCoreApplication::exit(-1);
    else
    {
      gamepad->initialize();
      cursor->setWindow(obj);
    }
  }, Qt::QueuedConnection);

  auto loadPackage = [&gameContext, &engine, &app, mainUrl](const GamePackage* package)
  {
    if (package)
    {
      if (gameContext)
        delete gameContext;
      gameContext = new GameContext(&app);
      gameContext->load(*package);
      gameContext->initializeQmlProperties(engine);
      engine.load(mainUrl);
    }
  };

  if (gamePackages->getPackages().size() == 1)
  {
    loadPackage(gamePackages->getPackages().first());
  }
  else
  {
    QObject::connect(gamePackages, &GamePackages::pickedPackage, &app, loadPackage, Qt::QueuedConnection);
    engine.load(gamePickerUrl);
  }
  return app.exec();
}
