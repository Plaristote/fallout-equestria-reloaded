#ifndef  SPEAKACTION_H
# define SPEAKACTION_H

# include "base.h"

class SpeakAction : public ActionBase
{
public:
  SpeakAction(Character* character, const QString& content, unsigned int duration, const QString& color);

  int getApCost() const{ return 0; }
  void update() override;
  bool trigger() override;

private:
  const QString content;
  const QString color;
  const unsigned int duration;
};

#endif // SPEAKACTION_H
