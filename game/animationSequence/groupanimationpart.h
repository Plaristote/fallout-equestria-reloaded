#ifndef GROUPANIMATIONPART_H
#define GROUPANIMATIONPART_H

#include "ianimationpart.h"

class GroupAnimationPart : public IAnimationPart
{
public:
  GroupAnimationPart();
  virtual ~GroupAnimationPart() override {}

  static bool matches(const QJSValue&);
  void initialize(QJSValue&) override;
  void start() override;
  bool isOver() const override;
  void addAnimationPart(IAnimationPart*);

private:
  QVector<QSharedPointer<IAnimationPart>> parts;
};

#endif // GROUPANIMATIONPART_H
