#include "globals.h"
#include "game.h"
#include "musicmanager.h"
#include "game/mousecursor.h"
#include "i18n.h"
#include <QFile>
#include <QDir>
#include "editor/leveleditorcontroller.h"
#include "cmap/perk.h"

Game* Game::instance = nullptr;

Game::Game(QObject *parent) : StorableObject(parent)
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
  taskManager = new TaskRunner(this);
  scriptEngine.installExtensions(QJSEngine::ConsoleExtension);
  scriptEngine.globalObject().setProperty("game", scriptEngine.newQObject(this));
  scriptEngine.globalObject().setProperty("quests", scriptEngine.newQObject(quests));
  scriptEngine.globalObject().setProperty("musicManager", scriptEngine.newQObject(MusicManager::get()));
  scriptEngine.globalObject().setProperty("i18n", scriptEngine.newQObject(I18n::get()));
  Race::initialize();
  Trait::initialize();
  Perk::initialize();
  scriptEngine.evaluate("level.displayConsoleMessage(\"Coucou Script Engine\")");

  connect(worldmap, &WorldMap::cityEntered, this, &Game::onCityEntered);
  connect(this, &Game::gameOver, this, &Game::onGameOver);
}

Game::~Game()
{
  scriptEngine.collectGarbage();
  if (instance == this)
    instance = nullptr;
  delete diplomacy;
  if (script)
    delete script;
}

void Game::deleteLater()
{
  if (currentLevel)
  {
    delete currentLevel;
    currentLevel = nullptr;
  }
  instance = nullptr;
  QObject::deleteLater();
}

QJSValue Game::eval(const QString& command)
{
  return scriptEngine.evaluate(command);
}

QString Game::consoleEval(const QString &command)
{
  return eval(command).toString();
}

void Game::newPlayerParty(StatModel* statistics)
{
  player = new Character;
  player->setObjectName("player");
  player->setStatistics(statistics);
  player->setScript("player.mjs");
  playerParty->addCharacter(player);
  connect(player->getInventory(), &Inventory::itemPicked, quests, &QuestManager::onItemPicked);
  connect(player, &Character::died, this, &Game::gameOver);
  initializeScript();
  if (script && script->hasMethod("initialize"))
    script->call("initialize");
}

void Game::onGameOver()
{
  if (player)
    disconnect(player, &Character::died, this, &Game::gameOver);
}

void Game::loadFromDataEngine()
{
  QString currentLevelName = dataEngine->getCurrentLevel();

  diplomacy->initialize();
  timeManager->load(dataEngine->getTimeData());
  playerParty->load(dataEngine->getPlayerParty());
  worldmap->load(dataEngine->getWorldmap());
  quests->load(dataEngine->getQuests());
  quests->addQuest("quest-test");
  player = playerParty->getCharacters().first();
  connect(player, &Character::died, this, &Game::gameOver);
  if (currentLevelName != "")
  {
    goToLevel(currentLevelName);
    playerParty->loadIntoLevel(currentLevel);
  }
  else
    emit levelChanged();
  dataStore = dataEngine->getVariables();
  initializeScript();
}

void Game::initializeScript()
{
  script = new ScriptController(SCRIPTS_PATH + "main.mjs");
  taskManager->setScriptController(script);
  script->initialize(this);
}

QStringList Game::getCharacterTemplateList() const
{
  QDir directory(ASSETS_PATH + "characterTemplates");
  QStringList results = directory.entryList(QStringList() << "*.json");

  for (QString& name : results)
    name = name.remove(".json");
  return results;
}

void Game::prepareEditor()
{
  getDataEngine()->loadFromFile("");
  worldmap->load(dataEngine->getWorldmap());
}

QJSValue Game::loadScript(const QString& path)
{
  QJSValue module = scriptEngine.importModule(path);

  if (module.isError())
  {
    qDebug() << "Couldn't load module" << path << ": uncaught exception at line "
             << module.property("lineNumber").toInt() << ":" << module.property("fileName").toString() << ":" << module.property("message").toString();
    return false;
  }
  return module;
}

void Game::onCityEntered(QString name)
{
  goToLevel(name);
  currentLevel->insertPartyIntoZone(playerParty);
}

void Game::onCityEnteredAt(const QString& name, const QString& zone)
{
  goToLevel(name);
  currentLevel->insertPartyIntoZone(playerParty, zone);
}

LevelTask* Game::newLevelTask()
{
  if (isGameEditor)
    return new LevelEditorController(this);
  return new LevelTask(this);
}

void Game::goToLevel(const QString& name)
{
  auto scriptObject = scriptEngine.globalObject();

  appendToConsole("You reached " + name);
  MusicManager::get()->play(name);
  dataEngine->setCurrentLevel(name);
  currentLevel = newLevelTask();
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
  currentLevel = newLevelTask();
  scriptObject.setProperty("level", scriptEngine.newQObject(currentLevel));
  connect(currentLevel, &LevelTask::displayConsoleMessage, this, &Game::appendToConsole);
  connect(currentLevel, &LevelTask::exitZoneEntered, this, &Game::changeZone);
  currentLevel->load(name, dataEngine);
  currentLevel->insertPartyIntoZone(playerParty, targetZone);
  currentLevel->setPaused(false);
  emit levelSwapped();
}

void Game::switchToEncounter(const QString &name, const QVariantMap &parameters)
{
  const QVariantList list = parameters.value("parties").toList();

  if (!currentLevel)
    goToLevel(name);
  else
    switchToLevel(name, "");
  currentLevel->setProperty("persistent", parameters.value("persistent", false));
  for (const QVariant& entry : list)
  {
    const QVariantMap partyData(entry.toMap());
    CharacterParty*   party = CharacterParty::factory(partyData, currentLevel);

    party->insertIntoZone(currentLevel, partyData.value("zone").toString());
  }
}

void Game::exitLevel(bool silent)
{
  if (currentLevel)
  {
    auto scriptObject = scriptEngine.globalObject();

    MusicManager::get()->play("worldmap");
    playerParty->extractFromLevel(currentLevel);
    currentLevel->save(dataEngine);
    currentLevel->deleteLater();
    currentLevel = nullptr;
    scriptObject.deleteProperty("level");
    dataEngine->exitLevel();
    MouseCursor::get()->updatePointerType();
    if (!silent)
      emit levelChanged();
  }
  else
    qDebug() << "Game::exitLevel called, but level is null";
}

void Game::changeZone(TileZone* tileZone)
{
  QString targetZone = tileZone->getTargetZone();

  if (tileZone->getTarget() != "")
  {
    if (targetZone.length() == 0 && currentLevel)
      targetZone = currentLevel->getName();
    switchToLevel(tileZone->getTarget(), targetZone);
  }
  else if (targetZone.length() > 0 && currentLevel)
  {
    playerParty->extractFromLevel(currentLevel);
    currentLevel->insertPartyIntoZone(playerParty, targetZone);
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
  dataEngine->setWorldmap(worldmap->save());
  dataEngine->setPlayerParty(partyData);
  dataEngine->setVariables(dataStore);
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

void Game::advanceTime(unsigned int minutes)
{
  timeManager->addElapsedMinutes(minutes);
  if (currentLevel)
    currentLevel->advanceTime(minutes);
  else
  {
    qint64 delta = minutes * 60 * 1000;

    for (Character* character : playerParty->getCharacters())
      character->updateTasks(delta);
    taskManager->update(delta);
    if (script)
      script->call("outdoorsTick");
  }
}
