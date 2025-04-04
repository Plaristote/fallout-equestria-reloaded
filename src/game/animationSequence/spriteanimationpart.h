#ifndef SPRITEANIMATIONPART_H
#define SPRITEANIMATIONPART_H

#include "ianimationpart.h"
#include <QPoint>
#include <QTimer>
#include <QJSValue>

class Sprite;
class VisualEffectsComponent;

class SpriteAnimationPart : public IAnimationPart
{
  QTimer                  timer;
protected:
  static const int        animationInterval = 38;
  Sprite*                 sprite;
  VisualEffectsComponent* level;
  bool                    over = false;
  QPoint                  from, to;
  int                     stepCount, stepIt = 0;
  QJSValue                position;

public:
  ~SpriteAnimationPart() override;

  static bool matches(const QJSValue&);

  void initialize(QJSValue& value) override;
  void start() override;
  bool isOver() const override { return over; }
  void onRefreshPosition();
  void onAnimationFinished() { over = true; }

private:
  void initializeMovement(QJSValue&);
};

#endif // SPRITEANIMATIONPART_H
