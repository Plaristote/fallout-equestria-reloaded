#ifndef  TIMEPASSER_H
# define TIMEPASSER_H

# include <QObject>
# include <QTimer>
# include <QJSValue>

class Game;

class TimePasser : public QObject
{
  Q_OBJECT

public:
  TimePasser(Game* game);

  bool isActive() const { return timer.isActive(); }
  void advanceTime(unsigned int minutes);
  void advanceTime(unsigned int minutes, QJSValue callback);

signals:
  void stateChanged();

private slots:
  void tick();

private:
  void triggerCallbacks();

  Game*        game;
  QTimer       timer;
  unsigned int remainingMinutes = 0;
  QList<QJSValue> callbacks;
};

#endif // TIMEPASSER_H
