#ifndef  WAITACTION_H
# define WAITACTION_H

# include "base.h"
# include "utils/datetime.hpp"

class WaitAction : public ActionBase
{
public:
  WaitAction(Character* character, unsigned int interval);

  int getApCost() const { return 0; }
  void update();
  bool trigger() override;

private:
  unsigned int interval;
  DateTime     endsAt;
};

#endif // WAITACTION_H
