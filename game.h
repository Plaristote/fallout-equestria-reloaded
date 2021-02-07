#ifndef  GAME_H
# define GAME_H

# include <QObject>
# include "game/leveltask.h"

class Game : public QObject
{
  Q_OBJECT

  Q_PROPERTY(LevelTask* level MEMBER currentLevel NOTIFY levelChanged)
  Q_PROPERTY(QStringList consoleMessages MEMBER consoleMessages NOTIFY consoleUpdated)
public:
  explicit Game(QObject *parent = nullptr);

  Q_INVOKABLE void appendToConsole(const QString&);
  Q_INVOKABLE void goToLevel(const QString& name);
  void exitLevel();

signals:
  void levelChanged();
  void consoleUpdated();

private:
  LevelTask* currentLevel = nullptr;
  QStringList consoleMessages;
};

#endif // GAME_H
