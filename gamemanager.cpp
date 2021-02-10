#include "gamemanager.h"
#include <QDir>

GameManager::GameManager(QObject *parent) : QObject(parent), currentGame(nullptr)
{

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
    list = directory.entryList(QStringList() << "*.json", QDir::NoFilter, QDir::Time | QDir::Reversed);
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
    emit currentGameChanged();
  }
  else
    qDebug() << "ERROR cannot start new game while another is still running";
}

void GameManager::loadGame(const QString& path)
{
  if (!currentGame)
  {
    currentGame = new Game(this);
    currentGame->getDataEngine()->loadFromFile(path + ".json");
    emit currentGameChanged();
  }
  else
    qDebug() << "ERROR cannot start new game while another is still running";
}

void GameManager::saveGame(const QString& path)
{
  currentGame->save();
  currentGame->getDataEngine()->saveToFile(path + ".json");
}

void GameManager::endGame()
{
  currentGame->deleteLater();
  currentGame = nullptr;
  emit currentGameChanged();
}
