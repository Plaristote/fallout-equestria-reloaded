#ifndef  LOOKACTION_H
# define LOOKACTION_H

# include "base.h"

class LookAction : public ActionBase
{
public:
  LookAction(Character*, QPoint);

  int getApCost() const { return 0; }
  void update() override;
  bool trigger() override;


private:
  QPoint target;
};

#endif // LOOKACTION_H
