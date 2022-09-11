#include "cursor.h"
#include "game/mousecursor.h"

CursorComponent::CursorComponent(QObject *parent) : ParentType(parent)
{
  connect(this, &CursorComponent::mouseModeChanged,   this, &CursorComponent::mouseStateChanged);
  connect(this, &CursorComponent::mouseStateChanged,  MouseCursor::get(), &MouseCursor::updatePointerType);
  connect(this, &CursorComponent::hoveredTileChanged, MouseCursor::get(), &MouseCursor::updatePointerType);
}

void CursorComponent::centerCursorOn(DynamicObject *object)
{
  if (object)
  {
    QPoint position = cameraOffset() + getClickableOffsetFor(object);

    MouseCursor::get()->setRelativePosition(position);
  }
}

QPoint CursorComponent::getClickableOffsetFor(const DynamicObject *target) const
{
  QPoint position = getAdjustedOffsetFor(target);
  const QImage& image = target->getImage();

  for (int x = 0 ; x < image.width() ; ++x)
  {
    for (int y = 0 ; y < image.height() ; ++y)
    {
      QPoint pixelPosition = position + QPoint(x, y);

      if (image.pixelColor(x, y) != Qt::transparent && getObjectAt(pixelPosition) == target)
        return pixelPosition;
    }
  }
  return position;
}

void CursorComponent::swapMouseMode()
{
  switch (mouseMode)
  {
    case WaitCursor:
      return ;
    case InteractionCursor:
    case TargetCursor:
      mouseMode = MovementCursor;
      break ;
    default:
      mouseMode = InteractionCursor;
      break ;
  }
  emit mouseModeChanged();
}

void CursorComponent::enableWaitingMode(bool active)
{
  if ((mouseMode != WaitCursor && active) || (mouseMode == WaitCursor && !active))
  {
    mouseMode = active ? WaitCursor : MovementCursor;
    emit mouseModeChanged();
  }
}

bool CursorComponent::isPotentialTarget(const DynamicObject& object) const
{
  DynamicObject& p = const_cast<DynamicObject&>(object);
  bool isLiveCharacter = object.isCharacter() && reinterpret_cast<const Character&>(object).isAlive();

  return !isLiveCharacter || visibleCharacters.indexOf(reinterpret_cast<Character*>(&p)) >= 0;
}

DynamicObject* CursorComponent::getObjectAt(int posX, int posY) const
{
  QVector<DynamicObject*> list;

  list.reserve(objectCount());
  eachObject([this, &list](DynamicObject* object)
  {
    if (object->getCurrentFloor() == getCurrentFloor() && (object->isCharacter() || object->hasInteractionOverlay()))
      list.push_back(object);
  });
  sortByRenderOrder(list);
  for (DynamicObject* object : qAsConst(list))
  {
    if (isPotentialTarget(*object))
    {
      QPoint coordinates = getAdjustedOffsetFor(object);
      QRect  clip = object->getClippedRect();
      QRect  boundingBox(coordinates, clip.size());

      if (posX >= boundingBox.x() && posX <= boundingBox.x() + boundingBox.width() &&
          posY >= boundingBox.y() && posY <= boundingBox.y() + boundingBox.height())
      {
        QPoint collisionAt(posX - coordinates.x(), posY - coordinates.y());
        QPoint sheetPosition(clip.x() + collisionAt.x(), clip.y() + collisionAt.y());
        const QImage& image = object->getImage();

        if (image.pixelColor(sheetPosition) != Qt::transparent)
           return object;
      }
    }
  }
  return nullptr;
}
