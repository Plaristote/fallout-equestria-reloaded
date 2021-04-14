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
  virtual bool trigger() override;
  void update() override;
  virtual void interrupt() override;

protected:
  virtual void triggerNextMovement();
  virtual void onMovementFinished();

  QPoint target;
private:
  bool firstRound;
};

#endif
