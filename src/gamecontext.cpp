#include "gamecontext.h"
#include "globals.h"
#include "i18n.h"
#include "gamepackages.h"
#include "gamemanager.h"
#include "musicmanager.h"
#include "game/soundmanager.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QCoreApplication>

QString preRenderRoot();

GameContext::GameContext(QObject *parent)
  : QObject{parent}, itemLibrary(this)
{}

void GameContext::load(const GamePackage& package)
{
  QCoreApplication::setOrganizationName(package.getOrganizationName());
  QCoreApplication::setOrganizationDomain(package.getOrganizationDomain());
  QCoreApplication::setApplicationName(package.getApplicationName());
  QDir::setCurrent(package.getGamePath());
  animationLibrary.initialize();
  itemLibrary.initialize();
  i18n         = new I18n(this);
  fileProtocol = FILE_PROTOCOL;
  rootPath     = FILE_PROTOCOL + ROOT_PATH;
  scriptPath   = FILE_PROTOCOL + SCRIPTS_PATH;
  assetPath    = FILE_PROTOCOL + ASSETS_PATH;
  musicManager = new MusicManager(this);
  soundManager = new SoundManager(this);
  gameManager  = new GameManager(this);
  soundManager->initialize();
  qDebug() << "Current directory: " << QDir::currentPath();
  qDebug() << "Using prerender directory " << preRenderRoot();
}

void GameContext::initializeQmlProperties(QQmlEngine& engine)
{
  engine.rootContext()->setContextProperty("i18n", i18n);
  engine.rootContext()->setContextProperty("gameManager", gameManager);
  engine.rootContext()->setContextProperty("musicManager", musicManager);
  engine.rootContext()->setContextProperty("soundManager", soundManager);
  engine.rootContext()->setContextProperty("animationLibrary", &animationLibrary);
  engine.rootContext()->setContextProperty("itemLibrary", &itemLibrary);
  engine.rootContext()->setContextProperty("fileProtocol", fileProtocol);
  engine.rootContext()->setContextProperty("rootPath", rootPath);
  engine.rootContext()->setContextProperty("scriptPath", scriptPath);
  engine.rootContext()->setContextProperty("assetPath", assetPath);
  QObject::connect(i18n, &I18n::currentLocaleChanged, this, [this, &engine]()
  {
    engine.rootContext()->setContextProperty("i18n", i18n);
  });
}
