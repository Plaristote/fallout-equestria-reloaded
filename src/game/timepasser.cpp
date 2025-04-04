#include "timepasser.h"
#include "game.h"

TimePasser::TimePasser(Game* game) : game(game)
{
  initializationTimer.setInterval(700);
  initializationTimer.setSingleShot(true);
  timer.setInterval(30);
  timer.setSingleShot(true);
  connect(&initializationTimer, &QTimer::timeout, this, &TimePasser::startTimePassing);
  connect(&timer, &QTimer::timeout, this, &TimePasser::tick);
}

void TimePasser::advanceTime(unsigned int minutes)
{
  if (minutes > remainingMinutes)
    remainingMinutes = remainingMinutes + (minutes - remainingMinutes);
  if (!timer.isActive() && remainingMinutes > 0)
  {
    initializationTimer.start();
    emit stateChanged();
  }
}

void TimePasser::startTimePassing()
{
  timer.start();
}

void TimePasser::advanceTime(unsigned int minutes, QJSValue callback)
{
  advanceTime(minutes);
  if (isActive() && callback.isCallable())
    callbacks << callback;
}

void TimePasser::tick()
{
  unsigned int minutes = remainingMinutes > 10 ? 10 : remainingMinutes;

  game->advanceTime(remainingMinutes > 10 ? 10 : remainingMinutes);
  remainingMinutes -= minutes;
  if (!remainingMinutes)
    triggerCallbacks();
  else
    timer.start();
}

QString jsErrorBacktrace(QJSValue retval);

void TimePasser::triggerCallbacks()
{
  for (auto callback : std::as_const(callbacks))
  {
    auto retval = callback.call();

    if (retval.isError())
      qDebug() << jsErrorBacktrace(retval);
  }
  callbacks.clear();
  emit stateChanged();
}
