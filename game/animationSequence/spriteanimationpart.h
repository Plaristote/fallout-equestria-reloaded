#ifndef SPRITEANIMATIONPART_H
#define SPRITEANIMATIONPART_H

#include "ianimationpart.h"
#include <QPoint>

class Sprite;
class LevelTask;

class SpriteAnimationPart : public IAnimationPart
{
protected:
  Sprite*    sprite;
  LevelTask* level;
  bool       over = false;
  QPoint     from, to;

public:
  ~SpriteAnimationPart() override;

  static bool matches(const QJSValue&);

  void initialize(QJSValue& value) override;
  void start() override;
  bool isOver() const override { return over; }
  void onAnimationFinished() { over = true; }
};

#endif // SPRITEANIMATIONPART_H
