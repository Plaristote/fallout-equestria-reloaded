#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "game/animationlibrary.h"

#include "tilemap/tilemap.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tile.h"

#include "game.h"
#include "game/dynamicobject.h"
#include "game/leveltask.h"

#include "cmap/statmodel.h"

void registerQmlTilemap() {
  qmlRegisterType<TileMap>  ("Tiles", 1,0, "TileMap");
  qmlRegisterType<TileLayer>("Tiles", 1,0, "TileLayer");
  qmlRegisterType<Tile>     ("Tiles", 1,0, "Tile");
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  AnimationLibrary animationLibrary;
  animationLibrary.initialize();

  Game* game = new Game();
  game->goToLevel("eltest5");

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  qmlRegisterType<Game>("Game", 1,0, "Controller");
  qmlRegisterType<StatModel>("Game", 1,0, "StatModel");
  qmlRegisterType<DynamicObject>("Game", 1,0, "DynamicObject");
  registerQmlTilemap();

  engine.rootContext()->setContextProperty("gameController", game);
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
