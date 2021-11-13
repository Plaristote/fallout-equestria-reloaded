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
  void canceled() override;

private:
  bool call(bool param);

  QJSValue callback;
};

#endif // SCRIPTACTION_H
