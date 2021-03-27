#ifndef  CMAPPLUGIN_H
# define CMAPPLUGIN_H

# include <QJSValue>

class StatModel;

class CmapPlugin
{
public:
  CmapPlugin();

  void toogle(StatModel*, bool) const;
  int modifyBaseStatistic(StatModel*, const QString& attribute, int baseValue) const;
  int modifyBaseSkill(StatModel*, const QString& attribute, int baseValue) const;

  QString  name;
  QJSValue script;
};

#endif // CMAPPLUGIN_H
