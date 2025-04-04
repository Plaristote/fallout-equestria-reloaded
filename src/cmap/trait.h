#ifndef  TRAIT_H
# define TRAIT_H

# include "cmap/plugin.h"

class Trait : public CmapPlugin
{
public:
  static void initialize();
  static const QMap<QString, Trait>& getTraits() { return traits; }

private:
  static QMap<QString, Trait> traits;
};

#endif // TRAIT_H
