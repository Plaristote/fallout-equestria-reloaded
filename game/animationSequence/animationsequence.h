#ifndef ANIMATIONSEQUENCE_H
# define ANIMATIONSEQUENCE_H

# include <QVector>
# include <QSharedPointer>

class QJSValue;
class IAnimationPart;

class AnimationSequence
{
public:
  void initialize(QJSValue& value);
  bool update();

private:
  bool nextAnimation();
  QVector<QSharedPointer<IAnimationPart>> parts;
};

#endif
