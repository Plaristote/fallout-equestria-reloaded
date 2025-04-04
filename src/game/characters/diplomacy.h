#ifndef  CHARACTERDIPLOMACY_H
# define CHARACTERDIPLOMACY_H

# include <QObject>
# include "../diplomacy.hpp"
# include "statistics.h"

class CharacterDiplomacy : public CharacterStatistics
{
  Q_OBJECT
  typedef CharacterStatistics ParentType;
public:
  explicit CharacterDiplomacy(QObject *parent = nullptr);

  Q_INVOKABLE QString getFactionName() const { return faction ? faction->name : QString(); }
  unsigned int getFactionFlag() const { return faction ? faction->flag : 0; }
  Q_INVOKABLE bool isAlly(const CharacterDiplomacy*) const;
  Q_INVOKABLE bool isEnemy(const CharacterDiplomacy*) const;
  Q_INVOKABLE void setAsEnemy(CharacterDiplomacy*);
  Q_INVOKABLE void setAsFriendly(CharacterDiplomacy*);
  Q_INVOKABLE int getReputation() const;
  Q_INVOKABLE void addReputation(int) const;

  void load(const QJsonObject&) override;
  void save(QJsonObject&) const override;

signals:
  void diplomacyUpdated();

private slots:
  void initializeFaction();

protected:
  WorldDiplomacy::Faction* faction = nullptr;
  unsigned long long       enemyFlag = 0;
  QMetaObject::Connection  factionListener;
};

#endif // CHARACTERDIPLOMACY_H
