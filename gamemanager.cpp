#include "gamemanager.h"

GameManager::GameManager(QObject *parent) : QObject(parent), currentGame(nullptr)
{

}

void GameManager::startNewGame()
{
  if (!currentGame)
  {
    currentGame = new Game(this);
    emit currentGameChanged();
  }
  else
    qDebug() << "ERROR cannot start new game while another is still running";
}

void GameManager::endGame()
{
  currentGame->deleteLater();
  currentGame = nullptr;
  emit currentGameChanged();
}
