#include "dynamicobject.h"
#include "game.h"

DynamicObject::DynamicObject(QObject *parent) : Sprite(parent)
{
  connect(&tick, &QTimer::timeout, this, &DynamicObject::onTicked);
  connect(this, &Sprite::movementFinished, this, &DynamicObject::onMovementEnded);
  connect(this, &DynamicObject::reachedDestination, this, &DynamicObject::onDestinationReached);
}


void DynamicObject::setTickBehavior(int interval, bool repeat)
{
  tick.setSingleShot(!repeat);
  tick.setInterval(interval);
  tick.start();
}

void DynamicObject::moveTo(int x, int y, QPoint renderPosition)
{
  QString direction, animationName;

  if (position.x() > x && position.y() > y)
    direction = "up";
  else if (position.x() < x && position.y() < y)
    direction = "down";
  else if (position.x() == x && position.y() > y)
    direction = "right";
  else if (position.x() == x && position.y() < y)
    direction = "left";
  else if (position.x() >= x)
    direction = "left";
  else
    direction = "right";
  animationName = "walking-" + direction;
  position = QPoint(x, y);
  moveToCoordinates(renderPosition);
  if (getCurrentAnimation() != animationName)
    setAnimation(animationName);
}

void DynamicObject::onMovementEnded()
{
  QJSValue callback = script.property("onMovementEnded");

  if (callback.isCallable())
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(this);
    Game::get()->scriptCall(callback, args, "onMovementEnded");
  }
  //position = nextPosition;
  //qDebug() << "new position" << position;
}

void DynamicObject::onDestinationReached()
{
  QJSValue callback = script.property("onDestinationReached");

  if (callback.isCallable())
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(this);
    Game::get()->scriptCall(callback, args, "onDestinationReached");
  }
}

void DynamicObject::onTicked()
{
  QJSValue callback = script.property("onTicked");

  if (callback.isCallable())
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(this);
    Game::get()->scriptCall(callback, args, "onTicked");
  }
}
