#ifndef  SCRIPTACTION_H
# define SCRIPTACTION_H

# include "base.h"

class ScriptAction : public ActionBase
{
public:
  ScriptAction(Character* character, QJSValue config);

  int getApCost() const override { return 0; }
  void update() override;
  bool trigger() override;
  void canceled() override;

private:
  bool call(QJSValue callback);

  QJSValue triggerCallback, cancelCallback;
};

#endif // SCRIPTACTION_H
