#ifndef  SCRIPTACTION_H
# define SCRIPTACTION_H

# include "base.h"

class ScriptAction : public ActionBase
{
public:
  ScriptAction(Character* character, QJSValue callback);

  int getApCost() const { return 0; }
  void update() override;
  bool trigger() override;

private:
  QJSValue callback;
};

#endif // SCRIPTACTION_H
