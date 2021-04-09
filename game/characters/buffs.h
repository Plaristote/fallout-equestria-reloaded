#ifndef  CHARACTERBUFFS_H
# define CHARACTERBUFFS_H

# include "sight.h"
# include "buff.h"

class CharacterBuffs : public CharacterSight
{
  typedef CharacterSight ParentType;
  Q_OBJECT
public:
  CharacterBuffs(QObject* parent = nullptr);

  virtual void updateTasks(qint64 delta) override;
  virtual void load(const QJsonObject&) override;
  virtual void save(QJsonObject&) const override;

  Q_INVOKABLE Buff* addBuff(const QString&);
  Q_INVOKABLE Buff* getBuff(const QString&) const;

public slots:
  void clearBuffs();
private slots:
  void onBuffAdded(Buff*);
  void onBuffRemoved(Buff*);

private:
  QVector<Buff*> buffs;
};

#endif // CHARACTERBUFFS_H
