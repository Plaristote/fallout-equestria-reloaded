#ifndef  TRAIT_H
# define TRAIT_H

# include <QJSValue>

class StatModel;

class Trait
{
public:
  Trait();

  void toogle(StatModel*, bool);
  int modifyBaseStatistic(StatModel*, const QString& attribute, int baseValue);
  int modifyBaseSkill(StatModel*, const QString& attribute, int baseValue);

  QString  name;
  QJSValue script;
};

#endif // TRAIT_H
