#ifndef  GAME_H
# define GAME_H

# include <QObject>
# include "game/leveltask.h"

class Game : public QObject
{
  Q_OBJECT

  Q_PROPERTY(LevelTask* level MEMBER currentLevel NOTIFY levelChanged)
public:
  explicit Game(QObject *parent = nullptr);

  void goToLevel(const QString& name);
  void exitLevel();

signals:
  void levelChanged();

private:
  LevelTask* currentLevel = nullptr;
};

#endif // GAME_H
