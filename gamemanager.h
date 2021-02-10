#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "game.h"

class GameManager : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Game* currentGame MEMBER currentGame NOTIFY currentGameChanged)
public:
  explicit GameManager(QObject *parent = nullptr);

  Q_INVOKABLE bool hasContinueGame() const;
  Q_INVOKABLE void startNewGame();
  Q_INVOKABLE void loadGame(const QString&);
  Q_INVOKABLE void saveGame(const QString&);
  Q_INVOKABLE void endGame();
  Q_INVOKABLE QStringList getSavedGames() const;

signals:
  void currentGameChanged();

private:
  Game* currentGame;
};

#endif // GAMEMANAGER_H
