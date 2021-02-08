#include "game.h"
#include <QFile>
#include <QDir>

Game* Game::instance = nullptr;

Game::Game(QObject *parent) : QObject(parent)
{
  if (instance != nullptr)
    throw std::runtime_error("can't have two Game instances at once");
  instance = this;
  scriptEngine.installExtensions(QJSEngine::ConsoleExtension);
  loadCmapTraits();
  scriptEngine.evaluate("level.displayConsoleMessage(\"Coucou Script Engine\")");
}

Game::~Game()
{
  scriptEngine.collectGarbage();
  instance = nullptr;
}

QJSValue Game::loadScript(const QString& path)
{
  auto module = scriptEngine.importModule(path);

  if (module.isError())
  {
    qDebug() << "Couldn't load module" << path << ": uncaught exception at line "
             << module.property("lineNumber").toInt() << ":" << module.toString();
    return false;
  }
  return module;
}

void Game::loadCmapTraits()
{
  QDir traitsDir(":/scripts/traits");
  auto files = traitsDir.entryList(QStringList() << "*.mjs" << "*.js", QDir::Files);
  qDebug() << traitsDir.entryList();
  for (auto scriptPath : files)
  {
    qDebug() << "Loading trait:" << scriptPath;
    auto script = loadScript(":/scripts/traits/" + scriptPath);

    if (!script.isBool())
    {
      Trait trait;

      trait.name = script.property("name").toString();
      trait.script = script;
      cmapTraits.insert(trait.name, trait);
    }
  }
}

void Game::goToLevel(const QString& name)
{
  auto scriptObject = scriptEngine.globalObject();

  appendToConsole("You reached " + name);
  currentLevel = new LevelTask(this);
  connect(currentLevel, &LevelTask::displayConsoleMessage, this, &Game::appendToConsole);
  currentLevel->load(name);
  scriptObject.setProperty("level", scriptEngine.newQObject(currentLevel));
  emit levelChanged();
}

void Game::exitLevel()
{
  currentLevel->deleteLater();
  currentLevel = nullptr;
  emit levelChanged();
}

void Game::appendToConsole(const QString& message)
{
  if (consoleMessages.size() > 100)
    consoleMessages.pop_front();
  consoleMessages << message;
  emit consoleUpdated();
  qDebug() << "[Console] :> " << message;
}
