#include "globals.h"
#include "game.h"
#include "musicmanager.h"
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
  diplomacy = new WorldDiplomacy(*dataEngine);
  playerParty = new CharacterParty(this);
  worldmap = new WorldMap(this);
  quests = new QuestManager(this);
  scriptEngine.installExtensions(QJSEngine::ConsoleExtension);
  scriptEngine.globalObject().setProperty("game", scriptEngine.newQObject(this));
  scriptEngine.globalObject().setProperty("quests", scriptEngine.newQObject(quests));
  scriptEngine.globalObject().setProperty("musicManager", scriptEngine.newQObject(MusicManager::get()));
  loadCmapTraits();
  scriptEngine.evaluate("level.displayConsoleMessage(\"Coucou Script Engine\")");

  connect(worldmap, &WorldMap::cityEntered, this, &Game::onCityEntered);
}

Game::~Game()
{
  scriptEngine.collectGarbage();
  if (instance == this)
    instance = nullptr;
  delete diplomacy;
}

void Game::deleteLater()
{
  instance = nullptr;
  QObject::deleteLater();
}

void Game::newPlayerParty(StatModel* statistics)
{
  player = new Character;
  player->setObjectName("player");
  player->setSpriteName("pony");
  player->setAnimation("idle");
  player->setStatistics(statistics);
  player->setScript("player.mjs");
  playerParty->addCharacter(player);
  connect(player->getInventory(), &Inventory::itemPicked, quests, &QuestManager::onItemPicked);
  connect(player, &Character::died, this, &Game::gameOver);
}

void Game::loadFromDataEngine()
{
  QString currentLevelName = dataEngine->getCurrentLevel();

  diplomacy->initialize();
  timeManager->load(dataEngine->getTimeData());
  playerParty->load(dataEngine->getPlayerParty());
  quests->load(dataEngine->getQuests());
  quests->addQuest("quest-test");
  player = playerParty->getCharacters().first();
  connect(player, &Character::died, this, &Game::gameOver);
  if (currentLevelName != "")
  {
    goToLevel(currentLevelName);
    playerParty->loadIntoLevel(currentLevel);
  }
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
  QDir traitsDir(SCRIPTS_PATH + "traits");
  auto files = traitsDir.entryList(QStringList() << "*.mjs" << "*.js", QDir::Files);
  qDebug() << traitsDir.entryList();
  for (auto scriptPath : files)
  {
    qDebug() << "Loading trait:" << scriptPath;
    auto script = loadScript(SCRIPTS_PATH + "traits/" + scriptPath);

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

void Game::goToLevel(const QString& name)
{
  auto scriptObject = scriptEngine.globalObject();

  appendToConsole("You reached " + name);
  MusicManager::get()->play(name);
  dataEngine->setCurrentLevel(name);
  currentLevel = new LevelTask(this);
  scriptObject.setProperty("level", scriptEngine.newQObject(currentLevel));
  connect(currentLevel, &LevelTask::displayConsoleMessage, this, &Game::appendToConsole);
  connect(currentLevel, &LevelTask::exitZoneEntered, this, &Game::changeZone);
  currentLevel->load(name, dataEngine);
  currentLevel->setPaused(false);
  emit levelChanged();
}

void Game::switchToLevel(const QString& name, const QString& targetZone)
{
  auto scriptObject = scriptEngine.globalObject();

  if (currentLevel)
    exitLevel(true);
  MusicManager::get()->play(name);
  dataEngine->setCurrentLevel(name);
  currentLevel = new LevelTask(this);
  scriptObject.setProperty("level", scriptEngine.newQObject(currentLevel));
  connect(currentLevel, &LevelTask::displayConsoleMessage, this, &Game::appendToConsole);
  connect(currentLevel, &LevelTask::exitZoneEntered, this, &Game::changeZone);
  currentLevel->load(name, dataEngine);
  currentLevel->insertPartyIntoZone(playerParty, targetZone);
  currentLevel->setPaused(false);
  emit levelSwapped();
}

void Game::exitLevel(bool silent)
{
  auto scriptObject = scriptEngine.globalObject();

  MusicManager::get()->play("worldmap");
  playerParty->extractFromLevel(currentLevel);
  currentLevel->save(dataEngine);
  currentLevel->deleteLater();
  currentLevel = nullptr;
  scriptObject.deleteProperty("level");
  dataEngine->exitLevel();
  if (!silent)
    emit levelChanged();
}

void Game::changeZone(TileZone* tileZone)
{
  if (tileZone->getTarget() != "")
  {
    QString targetZone;

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
  dataEngine->setQuests(quests->save());
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
