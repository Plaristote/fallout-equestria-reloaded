#ifndef  SLIDINGACTION_H
# define SLIDINGACTION_H

# include "base.h"
# include <QElapsedTimer>

class SlidingAction : public ActionBase
{
public:
  SlidingAction(Character* character, QPoint target);

  int  getApCost() const override { return 0; }
  void update() override;
  bool trigger() override;
  void interrupt() override;
  bool canInterrupt() const override { return false; }

private:
  bool   animationRunning() const;
  QPoint getTargetCoordinates() const;
  void   onFinished();

  QPoint target;
  QElapsedTimer timer;
};

#endif // SLIDINGACTION_H
