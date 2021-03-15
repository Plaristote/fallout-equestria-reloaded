#ifndef  TRAIT_H
# define TRAIT_H

# include <QJSValue>

class StatModel;

class CmapPlugin
{
public:
  CmapPlugin();

  void toogle(StatModel*, bool);
  int modifyBaseStatistic(StatModel*, const QString& attribute, int baseValue);
  int modifyBaseSkill(StatModel*, const QString& attribute, int baseValue);

  QString  name;
  QJSValue script;
};

typedef CmapPlugin Trait;

class Race : public CmapPlugin
{
public:
  bool isPlayable() const;
};

#endif // TRAIT_H
