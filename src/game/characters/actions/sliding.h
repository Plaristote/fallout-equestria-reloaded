#ifndef  SLIDINGACTION_H
# define SLIDINGACTION_H

# include "base.h"

class SlidingAction : public ActionBase
{
public:
  SlidingAction(Character* character, QPoint target);

  int  getApCost() const { return 0; }
  void update() override;
  bool trigger();
  void interrupt() override;
  bool canInterrupt() const override { return false; }

private:
  bool   animationRunning() const;
  QPoint getTargetCoordinates() const;
  void   onFinished();

  QPoint target;
};

#endif // SLIDINGACTION_H
