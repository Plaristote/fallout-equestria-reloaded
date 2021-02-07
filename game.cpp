#include "game.h"

Game::Game(QObject *parent) : QObject(parent)
{

}

void Game::goToLevel(const QString& name)
{
  appendToConsole("You reached " + name);
  currentLevel = new LevelTask(this);
  connect(currentLevel, &LevelTask::displayConsoleMessage, this, &Game::appendToConsole);
  currentLevel->load(name);
  emit levelChanged();
}

void Game::exitLevel()
{
  currentLevel->deleteLater();
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
