#ifndef  MOVEMENT_ACTION_H
# define MOVEMENT_ACTION_H

# include "base.h"
# include <QPoint>

class MovementAction : public ActionBase
{
public:
  MovementAction(Character* character, Point target) : ActionBase(character), target(target)
  {
  }

  int  getApCost() const override;
  virtual bool trigger() override;
  void update() override;
  virtual void interrupt() override;
  void setForced(bool value) { forced = value; }

protected:
  virtual void triggerNextMovement();
  virtual void onMovementFinished();
  int pathApCost(const QList<Point>&) const;
  bool canMakeNextMovement() const;

  Point target;
  bool forced = false;
private:
  bool firstRound = true;
};

#endif
