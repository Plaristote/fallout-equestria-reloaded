#ifndef ANIMATIONSEQUENCE_H
# define ANIMATIONSEQUENCE_H

# include <QVector>
# include <QSharedPointer>
# include <QElapsedTimer>

class QJSValue;
class IAnimationPart;

class AnimationSequence
{
public:
  void initialize(QJSValue& value);
  void addAnimationPart(IAnimationPart*);
  void start();
  bool update();
  bool isRunning() const;

private:
  bool nextAnimation();
  QVector<QSharedPointer<IAnimationPart>> parts;
  QElapsedTimer timer;
};

#endif
