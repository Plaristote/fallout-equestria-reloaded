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

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  void       setStatistics(StatModel* value) { statistics = value; }
  StatModel* getStatistics() { return statistics; }
  QString    getDialogName();
  bool       getIsUnique() const { return isUnique; }
  void       setUnique(bool value) { isUnique = value; }

  Q_INVOKABLE bool renderOnTile() const { return true; }
  Q_INVOKABLE QPoint getInteractionPosition() const;

signals:

protected:
  virtual QString getScriptPath() const { return ":/scripts/pnjs"; }
private:
  StatModel* statistics = nullptr;
  bool isUnique = false;
};

#endif // CHARACTER_H
