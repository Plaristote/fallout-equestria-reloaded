#include "dynamicobject.h"

DynamicObject::DynamicObject(QObject *parent) : Sprite(parent)
{
  connect(this, &Sprite::movementFinished, this, &DynamicObject::onMovementEnded);
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
  //position = nextPosition;
  //qDebug() << "new position" << position;
}
