#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "tilemap/tilemap.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tile.h"

#include "game.h"

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

  Game* game = new Game();
  game->goToLevel("eltest");

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  qmlRegisterType<Game>("Game", 1,0, "Controller");
  registerQmlTilemap();

  engine.rootContext()->setContextProperty("gameController", game);

  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl)
  {
    if (!obj && url == objUrl)
      QCoreApplication::exit(-1);
  }, Qt::QueuedConnection);
  engine.load(url);
  return app.exec();
}
