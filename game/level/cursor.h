#ifndef  CURSORCOMPONENT_H
# define CURSORCOMPONENT_H

# include "playermovement.h"

class CursorComponent : public PlayerMovementComponent
{
  Q_OBJECT
  typedef PlayerMovementComponent ParentType;

  Q_PROPERTY(int mouseMode MEMBER mouseMode NOTIFY mouseModeChanged)
  Q_PROPERTY(bool mouseInMap MEMBER mouseInMap NOTIFY mouseStateChanged)
  Q_PROPERTY(QPoint hoveredTile MEMBER hoveredTile NOTIFY hoveredTileChanged)
public:
  enum MouseMode
  {
    MovementCursor    = 0,
    InteractionCursor = 1,
    TargetCursor      = 2,
    WaitCursor        = 3
  };
  Q_ENUM(MouseMode)

  explicit CursorComponent(QObject *parent = nullptr);

  Q_INVOKABLE virtual void   swapMouseMode();
  MouseMode                  getMouseMode() const { return static_cast<MouseMode>(mouseMode); }
  QPoint                     getHoveredTilePosition() const { return hoveredTile; }
  bool                       mapIncludesMouse() const { return mouseInMap; }
  Q_INVOKABLE void           centerCursorOn(DynamicObject* object);
  Q_INVOKABLE void           enableWaitingMode(bool);
  Q_INVOKABLE DynamicObject* getObjectAt(int posX, int posY) const;
  DynamicObject*             getObjectAt(QPoint point) const { return getObjectAt(point.x(), point.y()); }


  QPoint getClickableOffsetFor(const DynamicObject* target) const;

signals:
  void mouseModeChanged();
  void mouseStateChanged();
  void hoveredTileChanged();

protected:
  int    mouseMode = MovementCursor;
  bool   mouseInMap = false;
  QPoint hoveredTile = QPoint(-1, -1);

};

#endif // CURSORCOMPONENT_H
