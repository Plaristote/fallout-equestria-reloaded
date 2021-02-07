#include "dynamicobject.h"

DynamicObject::DynamicObject(QObject *parent) : Sprite(parent)
{
  connect(this, &Sprite::movementFinished, this, &DynamicObject::onMovementEnded);
}

void DynamicObject::moveTo(int x, int y, QPoint renderPosition)
{
  position = QPoint(x, y);
  moveToCoordinates(renderPosition);
  setAnimation("walking-down");
}

void DynamicObject::onMovementEnded()
{
  //position = nextPosition;
  //qDebug() << "new position" << position;
}
