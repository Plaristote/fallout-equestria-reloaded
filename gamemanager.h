#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "game.h"

class GameManager : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Game* currentGame MEMBER currentGame NOTIFY currentGameChanged)
  Q_PROPERTY(int movementModeOption READ getMovementOption WRITE setMovementOption NOTIFY movementOptionChanged)
  Q_PROPERTY(double combatSpeedOption READ getCombatSpeedOption WRITE setCombatSpeedOption NOTIFY combatSpeedOptionChanged)
public:
  explicit GameManager(QObject *parent = nullptr);

  Q_INVOKABLE bool hasContinueGame() const;
  Q_INVOKABLE void startNewGame();
  Q_INVOKABLE void launchNewGame();
  Q_INVOKABLE void loadGame(const QString&);
  Q_INVOKABLE void saveGame(const QString&);
  Q_INVOKABLE void endGame();
  Q_INVOKABLE QStringList getSavedGames() const;

signals:
  void newGameStarted();
  void gameLoaded();
  void gameOver();
  void currentGameChanged();
  void movementOptionChanged();
  void combatSpeedOptionChanged();

private:
  int getMovementOption() const;
  void setMovementOption(int);
  double getCombatSpeedOption() const;
  void setCombatSpeedOption(double);

  Game* currentGame;
};

#endif // GAMEMANAGER_H
