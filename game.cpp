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
const QString nullTargetZone("_load");

bool shouldSaveTasks()
{
  return Game::get() && !Game::get()->property("isGameEditor").toBool();
}

bool shouldSaveVariables()
{
  return !Game::get() || !Game::get()->property("isGameEditor").toBool();
}

Game::Game(QObject *parent) : StorableObject(parent), timePasser(this)
{
  if (instance != nullptr)
    throw std::runtime_error("can't have two Game instances at once");
  instance = this;
  dataEngine = new DataEngine(this);
  timeManager = new TimeManager(this);
  diplomacy = new WorldDiplomacy(*dataEngine);
  playerParty = new CharacterParty(this);
  worldmap = new WorldMap(this);
  randomEncounters = new RandomEncounterController(this);
  uniqueCharacterStorage = new UniqueCharacterStorage(this);
  quests = new QuestManager(this);
  taskManager = new TaskRunner(this);
  soundManager = new SoundManager(this);
  scriptEngine.installExtensions(QJSEngine::ConsoleExtension);
  scriptEngine.globalObject().setProperty("game", scriptEngine.newQObject(this));
  scriptEngine.globalObject().setProperty("worldmap", scriptEngine.newQObject(worldmap));
  scriptEngine.globalObject().setProperty("quests", scriptEngine.newQObject(quests));
  scriptEngine.globalObject().setProperty("musicManager", scriptEngine.newQObject(MusicManager::get()));
  scriptEngine.globalObject().setProperty("i18n", scriptEngine.newQObject(I18n::get()));
  Race::initialize();
  Trait::initialize();
  Perk::initialize();
  scriptEngine.evaluate("level.displayConsoleMessage(\"Coucou Script Engine\")");

  connect(worldmap, &WorldMap::cityEntered, this, &Game::onCityEntered);
  connect(&timePasser, &TimePasser::stateChanged, this, &Game::fastPassingChanged);
  connect(this, &Game::gameOver, this, &Game::onGameOver);
  connect(this, &Game::levelChanged, this, &Game::onLevelChanged);
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
    destroyLevelTask();
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
  initializeEvents();
  initializeScript();
  if (script && script->hasMethod("initialize"))
    script->call("initialize");
}

void Game::onLevelChanged()
{
  if (script && script->hasMethod("onLevelChanged"))
    script->call("onLevelChanged");
}

void Game::onGameOver()
{
  if (player)
    disconnect(player, &Character::died, this, &Game::gameOver);
  disconnect(diplomacy, &WorldDiplomacy::update, this, &Game::onDiplomacyUpdate);
}

void Game::initializeEvents()
{
  connect(diplomacy, &WorldDiplomacy::update, this, &Game::onDiplomacyUpdate);
  connect(player->getInventory(), &Inventory::itemPicked, quests, &QuestManager::onItemPicked);
  connect(player, &Character::died, this, &Game::gameOver);
  connect(player->getStatistics(), &StatModel::levelChanged, this, [this]() { if (player->getStatistics()->property("level").toInt() > 1) soundManager->play("pipbuck/levelup"); });
}

void Game::loadFromDataEngine()
{
  QString currentLevelName = dataEngine->getCurrentLevel();

  diplomacy->initialize();
  timeManager->load(dataEngine->getTimeData());
  playerParty->load(dataEngine->getPlayerParty());
  uniqueCharacterStorage->load(dataEngine->getUniqueCharacterStorage());
  worldmap->load(dataEngine->getWorldmap());
  quests->load(dataEngine->getQuests());
  quests->addQuest("quest-test");
  player = playerParty->getCharacters().first();
  initializeEvents();
  if (currentLevelName != "")
    loadLevel(currentLevelName, nullTargetZone);
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
  switchToLevel(name, "");
}

void Game::onCityEnteredAt(const QString& name, const QString& zone)
{
  switchToLevel(name, zone);
}

LevelTask* Game::newLevelTask()
{
  if (isGameEditor)
    return new LevelEditorController(this);
  return new LevelTask(this);
}

void Game::loadLevel(const QString &name, const QString& targetZone)
{
  QTimer* timer;

  if (player && !player->isAlive())
    return ;
  emit requestLoadingScreen();
  timer = new QTimer(this);
  timer->start(500);
  connect(timer, &QTimer::timeout, timer, &QObject::deleteLater);
  connect(timer, &QTimer::timeout, this, [this, name, targetZone]()
  {
    auto scriptObject = scriptEngine.globalObject();

    setProperty("saveLock", false);
    MusicManager::get()->play(name);
    dataEngine->setCurrentLevel(name);
    currentLevel = newLevelTask();
    scriptObject.setProperty("level", scriptEngine.newQObject(currentLevel));
    connect(currentLevel, &LevelTask::displayConsoleMessage, this, &Game::appendToConsole);
    connect(currentLevel, &LevelTask::exitZoneEntered, this, &Game::changeZone, Qt::QueuedConnection);
    try
    {
      currentLevel->load(name, dataEngine);
      currentLevel->setPaused(false);
      if (targetZone == nullTargetZone)
      {
        playerParty->loadIntoLevel(currentLevel);
        uniqueCharacterStorage->loadUniqueCharactersToLevel(currentLevel);
      }
      else if (!isGameEditor)
      {
        currentLevel->insertPartyIntoZone(playerParty, targetZone);
        uniqueCharacterStorage->loadUniqueCharactersToLevel(currentLevel);
        currentLevel->scriptCall("onLoaded");
      }
      else
        uniqueCharacterStorage->loadUniqueCharactersToLevel(currentLevel);
    }
    catch (const std::runtime_error& error)
    {
      delete currentLevel;
      currentLevel = nullptr;
      emit loadError(QString(error.what()));
    }
    emit levelChanged();
  });
}

void Game::switchToLevel(const QString& name, const QString& targetZone)
{
  auto function = std::bind(&Game::loadLevel, this, name, targetZone);
  static QMetaObject::Connection listener;

  if (exitingLevel)
  {
    disconnect(listener);
    listener = connect(currentLevel, &QObject::destroyed, function);
  }
  else if (currentLevel)
  {
    listener = connect(currentLevel, &QObject::destroyed, function);
    exitLevel(true);
  }
  else
    function();
}

void Game::switchToCity(const QString& name, const QString& levelName, const QString& targetZone)
{
  WorldMapCity* city = worldmap->getCity(name);

  if (city)
  {
    QString targetLevel = levelName.isEmpty() ? city->getLevel() : levelName;

    worldmap->moveToCity(city);
    switchToLevel(targetLevel, targetZone);
  }
  else
    qDebug() << "Game::switchToCity: " << name << " is not a city name";
}

void Game::destroyLevelTask()
{
  emit levelDestroy();
  currentLevel->deleteLater();
  currentLevel = nullptr;
}

void Game::exitLevel(bool silent)
{
  if (currentLevel)
  {
    auto* timer = new QTimer(this);

    emit requestLoadingScreen();
    exitingLevel = true;
    timer->setInterval(500);
    timer->start();
    connect(timer, &QTimer::timeout, timer, &QObject::deleteLater);
    connect(timer, &QTimer::timeout, this, [this, silent]()
    {
      auto scriptObject = scriptEngine.globalObject();

      MusicManager::get()->play("worldmap");
      disconnect(currentLevel, &LevelTask::displayConsoleMessage, this, &Game::appendToConsole);
      disconnect(currentLevel, &LevelTask::exitZoneEntered, this, &Game::changeZone);
      playerParty->extractFromLevel(currentLevel);
      uniqueCharacterStorage->saveUniqueCharactersFromLevel(currentLevel);
      currentLevel->onExit();
      currentLevel->save(dataEngine);
      destroyLevelTask();
      scriptObject.deleteProperty("level");
      dataEngine->exitLevel();
      MouseCursor::get()->updatePointerType();
      setProperty("saveLock", false);
      if (!silent)
        emit levelChanged();
      exitingLevel = false;
    });
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
  QJsonObject partyData, timeData, uniqueCharactersData;

  timeManager->save(timeData);
  playerParty->save(partyData);
  uniqueCharacterStorage->save(uniqueCharactersData);
  if (currentLevel)
    currentLevel->save(dataEngine);
  dataEngine->setTimeData(timeData);
  dataEngine->setQuests(quests->save());
  dataEngine->setWorldmap(worldmap->save());
  dataEngine->setPlayerParty(partyData);
  dataEngine->setUniqueCharacterStorage(uniqueCharactersData);
  dataEngine->setVariables(dataStore);
}

void Game::setFactionAsEnemy(const QString& a, const QString& b, bool set)
{
  diplomacy->setAsEnemy(set, a, b);
}

void Game::onDiplomacyUpdate(const QStringList& factions, bool enemy)
{
  if (script->hasMethod("diplomacyUpdate"))
  {
    QJSValue array = scriptEngine.newArray();

    for (const auto& faction : factions)
      array.property("push").callWithInstance(array, QJSValueList() << faction);
    script->call("diplomacyUpdate", QJSValueList() << array << enemy);
  }
}

// TODO replace  with scriptcontroller::callvunction
QJSValue Game::scriptCall(QJSValue callable, const QJSValueList& args, const QString&)
{
  return ScriptController::callFunction(callable, args);
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
  timeManager->addElapsedMinutes(static_cast<int>(minutes));
  if (currentLevel)
    currentLevel->advanceTime(minutes);
  else
  {
    qint64 delta = minutes * 60 * 1000;

    for (Character* character : playerParty->getCharacters())
      character->updateTasks(delta);
    taskManager->update(delta);
    if (script)
      script->call("outdoorsTick", QJSValueList() << minutes);
  }
}

void Game::asyncAdvanceTime(unsigned int minutes)
{
  timePasser.advanceTime(minutes);
}

void Game::asyncAdvanceTime(unsigned int minutes, QJSValue callback)
{
  timePasser.advanceTime(minutes, callback);
}

Character* Game::getCharacter(const QString& characterSheet) const
{
  Character* match = nullptr;

  if (currentLevel)
    match = currentLevel->findCharacter([characterSheet](Character& character) { return character.getCharacterSheet() == characterSheet; });
  return match ? match : uniqueCharacterStorage->getCharacter(characterSheet);
}
