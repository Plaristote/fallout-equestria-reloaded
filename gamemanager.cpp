#include "gamemanager.h"
#include <QDir>
#include <QDebug>

GameManager::GameManager(QObject *parent) : QObject(parent), currentGame(nullptr)
{
  connect(this, &GameManager::newGameStarted, this, &GameManager::currentGameChanged);
  connect(this, &GameManager::gameLoaded,     this, &GameManager::currentGameChanged);
  connect(this, &GameManager::gameOver,       this, &GameManager::currentGameChanged);
}

bool GameManager::hasContinueGame() const
{
  return getSavedGames().count() > 0;
}

QStringList GameManager::getSavedGames() const
{
  QDir directory("./saves");
  QStringList list;

  if (directory.exists())
  {
    list = directory.entryList(QStringList() << "*.json", QDir::NoFilter, QDir::Time);
    for (auto it = list.begin() ; it != list.end() ; ++it)
      it->replace(".json", "");
  }
  return list;
}

void GameManager::startNewGame()
{
  if (!currentGame)
  {
    StatModel* playerStats;

    currentGame = new Game(this);
    playerStats = new StatModel(currentGame);
    currentGame->newPlayerParty(playerStats);
    currentGame->getDataEngine()->loadFromFile("");
    currentGame->getDiplomacy()->initialize();
    connect(currentGame->getDiplomacy(), &WorldDiplomacy::update, currentGame, &Game::onDiplomacyUpdate);
    // TODO initialize other factions
    currentGame->getWorldmap()->load(currentGame->getDataEngine()->getWorldmap());
    currentGame->getPlayer()->getStatistics()->setProperty("faction", "player");
    emit newGameStarted();
  }
  else
    qDebug() << "ERROR cannot start new game while another is still running";
}

void GameManager::launchNewGame()
{
  QJSValue   initializeScript, initializeFunction;

  currentGame->getPlayer()->updateSpriteSheet();
  initializeScript   = currentGame->loadScript(SCRIPTS_PATH + "initialize.mjs");
  initializeFunction = initializeScript.property("initialize");
  if (initializeFunction.isCallable())
    currentGame->scriptCall(initializeFunction, QJSValueList(), "initialize.mjs");
  else
    qDebug() << "Missing initialize function in scripts/initialize.mjs";
  if (!currentGame->getLevel())
    emit currentGame->levelChanged();
}

void GameManager::loadGame(const QString& path)
{
  endGame();
  currentGame = new Game(this);
  currentGame->getDataEngine()->loadFromFile(path + ".json");
  currentGame->loadFromDataEngine();
  emit gameLoaded();
}

void GameManager::saveGame(const QString& path)
{
  emit currentGame->requireScreenshot("./saves/" + path + ".png");
  currentGame->save();
  currentGame->getDataEngine()->saveToFile(path + ".json");
}

void GameManager::endGame()
{
  if (currentGame)
  {
    currentGame->deleteLater();
    currentGame = nullptr;
    emit gameOver();
  }
}

int GameManager::getMovementOption() const
{
  return LevelTask::movementModeOption;
}

void GameManager::setMovementOption(int value)
{
  if (value != LevelTask::movementModeOption)
  {
    LevelTask::movementModeOption = value;
    emit movementOptionChanged();
  }
}

double GameManager::getCombatSpeedOption() const
{
  return LevelTask::combatSpeedOption;
}

void GameManager::setCombatSpeedOption(double value)
{
  if (value != LevelTask::combatSpeedOption)
  {
    LevelTask::combatSpeedOption = value;
    emit combatSpeedOptionChanged();
  }
}
