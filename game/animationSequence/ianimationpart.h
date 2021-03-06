#ifndef  IANIMATIONPART_H
# define IANIMATIONPART_H

# include <QObject>

class QJSValue;

class IAnimationPart : public QObject
{
public:
  virtual ~IAnimationPart() {}

  virtual void initialize(QJSValue&) = 0;
  virtual void start() = 0;
  virtual void finish() {};
  virtual bool isOver() const = 0;
};

#endif // IANIMATIONPART_H
