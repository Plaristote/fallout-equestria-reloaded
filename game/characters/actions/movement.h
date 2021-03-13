#ifndef  MOVEMENT_ACTION_H
# define MOVEMENT_ACTION_H

# include "base.h"
# include <QPoint>

class MovementAction : public ActionBase
{
public:
  MovementAction(Character* character, QPoint target) : ActionBase(character), target(target)
  {
  }

  int  getApCost() const override;
  bool trigger() override;
  void update() override;
  void interrupt() override;

protected:
  void triggerNextMovement();

  QPoint target;
};

#endif
