#ifndef  DIPLOMACY_HPP
# define DIPLOMACY_HPP

# include <QObject>
# include <QString>
# include "dataengine.h"

class WorldDiplomacy : public QObject
{
  Q_OBJECT
public:
  struct Faction
  {
    QString      name;
    unsigned int flag;
    unsigned int enemyMask;

    bool operator==(const QString& name)
    { return (this->name == name); }

    bool operator==(unsigned int flag)
    { return (this->flag == flag); }
  };

  typedef std::list<Faction> Factions;

  WorldDiplomacy(DataEngine&);

  void     addFaction(const QString& name);
  Faction* getFaction(const QString& name);
  Faction* getFaction(unsigned int flag);

  void     setAsEnemy(bool set, const QString& name1, const QString& name2);
  void     setAsEnemy(bool set, unsigned int flag1, unsigned int flag2);

  void     initialize(void);

signals:
  void     update(const QStringList& factions, bool enemy);

private:
  void     setAsEnemy(bool set, Faction& first, Faction& second);

  DataEngine&  _data_engine;
  Factions     _factions;
  unsigned int _next_flag;
};

#endif


