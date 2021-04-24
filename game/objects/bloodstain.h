#ifndef  BLOODSTAIN_H
# define BLOODSTAIN_H

# include "../dynamicobject.h"

class BloodStain : public DynamicObject
{
  Q_OBJECT
public:
  explicit BloodStain(QObject *parent = nullptr);

  void initialize();
  int getZIndex() const override { return 0; }

private slots:
  void onAnimationEnded();
};

#endif // BLOODSTAIN_H
