#ifndef  CHARACTER_H
# define CHARACTER_H

# include "dynamicobject.h"
# include "cmap/statmodel.h"

class Character : public DynamicObject
{
  Q_OBJECT

  Q_PROPERTY(StatModel* statistics MEMBER statistics)
public:
  explicit Character(QObject *parent = nullptr);

  void setScript(const QString& name);

  StatModel* getStatistics() { return statistics; }

signals:

private:
  StatModel* statistics;
};

#endif // CHARACTER_H
