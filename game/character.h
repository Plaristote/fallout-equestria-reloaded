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

  StatModel* getStatistics() { return statistics; }

  Q_INVOKABLE bool renderOnTile() const { return true; }

signals:

protected:
  virtual QString getScriptPath() const { return ":/scripts/pnjs"; }
private:
  StatModel* statistics;
};

#endif // CHARACTER_H
