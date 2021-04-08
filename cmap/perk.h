#ifndef  PERK_H
# define PERK_H

# include "plugin.h"
# include <QList>

class StatModel;

class Perk : public CmapPlugin
{
public:
  static void initialize();
  static const QMap<QString, Perk>& getPerks() { return perks; }
  static QStringList                getAvailablePerks(StatModel*);

private:
  static QMap<QString, Perk> perks;
};

#endif // PERK_H
