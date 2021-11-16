#include "timepasser.h"
#include "game.h"

TimePasser::TimePasser(Game* game) : game(game)
{
  timer.setInterval(30);
  timer.setSingleShot(true);
  connect(&timer, &QTimer::timeout, this, &TimePasser::tick);
}

void TimePasser::advanceTime(unsigned int minutes)
{
  if (minutes > remainingMinutes)
    remainingMinutes = remainingMinutes + (minutes - remainingMinutes);
  if (!timer.isActive() && remainingMinutes > 0)
  {
    timer.start();
    emit stateChanged();
  }
}

void TimePasser::advanceTime(unsigned int minutes, QJSValue callback)
{
  advanceTime(minutes);
  if (timer.isActive() && callback.isCallable())
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

void TimePasser::triggerCallbacks()
{
  for (auto callback : callbacks)
    callback.call();
  callbacks.clear();
  emit stateChanged();
}
