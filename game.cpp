#include "game.h"
#include <QFile>
#include <QDir>

Game* Game::instance = nullptr;

Game::Game(QObject *parent) : QObject(parent)
{
  if (instance != nullptr)
    throw std::runtime_error("can't have two Game instances at once");
  instance = this;
  dataEngine = new DataEngine(this);
  timeManager = new TimeManager(this);
  playerParty = new CharacterParty(this);
  worldmap = new WorldMap(this);
  scriptEngine.installExtensions(QJSEngine::ConsoleExtension);
  scriptEngine.globalObject().setProperty("game", scriptEngine.newQObject(this));
  loadCmapTraits();
  scriptEngine.evaluate("level.displayConsoleMessage(\"Coucou Script Engine\")");

  connect(worldmap, &WorldMap::cityEntered, this, &Game::onCityEntered);
}

Game::~Game()
{
  scriptEngine.collectGarbage();
  if (instance == this)
    instance = nullptr;
}

void Game::deleteLater()
{
  instance = nullptr;
  QObject::deleteLater();
}

void Game::newPlayerParty(StatModel* statistics)
{
  player = new Character;
  player->setSpriteName("pony");
  player->setAnimation("idle-down");
  player->setStatistics(statistics);
  playerParty->addCharacter(player);
}

void Game::loadFromDataEngine()
{
  QString currentLevelName = dataEngine->getCurrentLevel();

  timeManager->load(dataEngine->getTimeData());
  if (currentLevelName != "")
    goToLevel(currentLevelName);
  playerParty->load(dataEngine->getPlayerParty(), currentLevel);
  player = playerParty->getCharacters().first();
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

void Game::onCityEntered(QString name)
{
  goToLevel(name);
  currentLevel->insertPartyIntoZone(playerParty);
}

// Keep in mind that Player is not available during Level loading
void Game::goToLevel(const QString& name)
{
  auto scriptObject = scriptEngine.globalObject();

  appendToConsole("You reached " + name);
  dataEngine->setCurrentLevel(name);
  currentLevel = new LevelTask(this);
  connect(currentLevel, &LevelTask::displayConsoleMessage, this, &Game::appendToConsole);
  connect(currentLevel, &LevelTask::exitZoneEntered, this, &Game::changeZone);
  currentLevel->load(name, dataEngine);
  currentLevel->setPaused(false);
  scriptObject.setProperty("level", scriptEngine.newQObject(currentLevel));
  emit levelChanged();
}

void Game::switchToLevel(const QString& name, const QString& targetZone)
{
  auto scriptObject = scriptEngine.globalObject();

  if (currentLevel)
    exitLevel(true);
  dataEngine->setCurrentLevel(name);
  currentLevel = new LevelTask(this);
  connect(currentLevel, &LevelTask::displayConsoleMessage, this, &Game::appendToConsole);
  connect(currentLevel, &LevelTask::exitZoneEntered, this, &Game::changeZone);
  currentLevel->load(name, dataEngine);
  currentLevel->insertPartyIntoZone(playerParty);
  currentLevel->setPaused(false);
  scriptObject.setProperty("level", scriptEngine.newQObject(currentLevel));
  emit levelSwapped();
}

void Game::exitLevel(bool silent)
{
  playerParty->extractFromLevel(currentLevel);
  currentLevel->save(dataEngine);
  currentLevel->deleteLater();
  currentLevel = nullptr;
  dataEngine->exitLevel();
  if (!silent)
    emit levelChanged();
}

void Game::changeZone(TileZone* tileZone)
{
  QString targetZone;

  if (tileZone->getTarget() != "")
  {
    if (currentLevel)
      targetZone = currentLevel->getName();
    switchToLevel(tileZone->getTarget(), targetZone);
  }
  else
    exitLevel();
}

void Game::save()
{
  QJsonObject partyData, timeData;

  timeManager->save(timeData);
  playerParty->save(partyData);
  if (currentLevel)
    currentLevel->save(dataEngine);
  dataEngine->setTimeData(timeData);
  dataEngine->setPlayerParty(partyData);
}

QJSValue Game::scriptCall(QJSValue callable, const QJSValueList& args, const QString& scriptName)
{
  QJSValue retval = callable.call(args);

  if (retval.isError())
  {
    qDebug() << "Script crashed:" << scriptName << ": uncaught exception at line "
             << retval.property("lineNumber").toInt() << ":" << retval.toString();
    return false;
  }

  return retval;
}

void Game::appendToConsole(const QString& message)
{
  if (consoleMessages.size() > 100)
    consoleMessages.pop_front();
  consoleMessages << message;
  emit consoleUpdated();
  qDebug() << "[Console] :> " << message;
}
