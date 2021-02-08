#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "game/animationlibrary.h"

#include "tilemap/tilemap.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tilezone.h"
#include "tilemap/tile.h"

#include "game.h"
#include "game/dynamicobject.h"
#include "game/leveltask.h"
#include "gamemanager.h"

#include "cmap/statmodel.h"

void registerQmlTilemap() {
  qmlRegisterType<TileMap>  ("Tiles", 1,0, "TileMap");
  qmlRegisterType<TileLayer>("Tiles", 1,0, "TileLayer");
  qmlRegisterType<Tile>     ("Tiles", 1,0, "Tile");
  qmlRegisterType<TileZone> ("Tiles", 1,0, "TileZone");
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  AnimationLibrary animationLibrary;
  animationLibrary.initialize();

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

/*
  QJSEngine jengine;
  QJSValue value = jengine.importModule("test.mjs");

  jengine.globalObject().setProperty("window", value);

  auto zz = value.toVariant();
  qDebug() << "koko:" << zz << value.isObject();
  auto ll = zz.toMap();

  if (zz.typeName())
    qDebug() << zz.typeName();
  for (auto& ii : ll)
  {
    qDebug() << ii.typeName();
  }

  jengine.setProperty("tintin", "Taratata");

  auto retval = jengine.evaluate("window.onTraitToggled(true)");

  qDebug() << "coucou" << retval.toString();
  return 0;
*/

  qmlRegisterType<Game>("Game", 1,0, "Controller");
  qmlRegisterType<StatModel>("Game", 1,0, "StatModel");
  qmlRegisterType<DynamicObject>("Game", 1,0, "DynamicObject");
  qmlRegisterType<GameManager>("Game", 1,0, "GameManager");
  registerQmlTilemap();

  GameManager* gameManager = new GameManager();

  engine.rootContext()->setContextProperty("gameManager", gameManager);
  engine.rootContext()->setContextProperty("animationLibrary", &animationLibrary);

  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl)
  {
    if (!obj && url == objUrl)
      QCoreApplication::exit(-1);
  }, Qt::QueuedConnection);
  engine.load(url);
  return app.exec();
}
