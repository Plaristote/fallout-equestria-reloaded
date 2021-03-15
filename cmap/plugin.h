#ifndef  CMAPPLUGIN_H
# define CMAPPLUGIN_H

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

#endif // CMAPPLUGIN_H
