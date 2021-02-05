#include "game.h"

Game::Game(QObject *parent) : QObject(parent)
{

}

void Game::goToLevel(const QString& name)
{
  currentLevel = new LevelTask(this);
  currentLevel->load(name);
  emit levelChanged();
}

void Game::exitLevel()
{
  currentLevel->deleteLater();
  emit levelChanged();
}
