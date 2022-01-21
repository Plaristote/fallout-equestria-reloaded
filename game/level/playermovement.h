#ifndef  PLAYERMOVEMENTCOMPONENT_H
# define PLAYERMOVEMENTCOMPONENT_H

# include "playervisibility.h"

class PlayerMovementComponent : public PlayerVisibilityComponent
{
  Q_OBJECT
  typedef PlayerVisibilityComponent ParentType;
public:
  static int movementModeOption;

  enum MovementMode
  {
    MixedMovementMode = 0,
    RunMovementMode   = 1,
    WalkMovementMode  = 2
  };
  Q_ENUM(MovementMode)

  explicit PlayerMovementComponent(QObject *parent = nullptr);

  void             setDefaultMovementMode();
  Q_INVOKABLE void movePlayerTo(int x, int y);
  Q_INVOKABLE QJSValue previewPathTo(int x, int y);

signals:
  void playerMovingTo(QPoint);
};

#endif // PLAYERMOVEMENTCOMPONENT_H
