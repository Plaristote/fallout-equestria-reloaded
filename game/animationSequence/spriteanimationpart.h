#ifndef SPRITEANIMATIONPART_H
#define SPRITEANIMATIONPART_H

#include "ianimationpart.h"
#include <QPoint>

class Sprite;
class LevelTask;

class SpriteAnimationPart : public IAnimationPart
{
  Q_OBJECT

protected:
  Sprite*    sprite;
  LevelTask* level;
  bool       over = false;
  QPoint     from, to;

public:
  ~SpriteAnimationPart();

  static bool matches(const QJSValue&);

  void initialize(QJSValue& value) override;
  void start() override;
  bool isOver() const override { return over; }

public slots:
  void onAnimationFinished() { over = true; }
};

#endif // SPRITEANIMATIONPART_H
