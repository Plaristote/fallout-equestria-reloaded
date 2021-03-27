#include "trait.h"
#include "pluginloader.h"

QMap<QString, Trait> Trait::traits;

void Trait::initialize()
{
  traits.clear();
  traits = CmapPlugins::load<Trait>(SCRIPTS_PATH + "cmap/traits");
}
