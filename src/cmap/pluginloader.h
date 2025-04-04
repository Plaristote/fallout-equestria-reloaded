#ifndef  PLUGINLOADER_H
# define PLUGINLOADER_H

# include <QDir>
# include <QMap>
# include "game.h"

class CmapPlugins
{
public:
  template<typename TYPE>
  static QMap<QString, TYPE> load(const QString& path)
  {
    QMap<QString, TYPE> results;
    QDir  dir(path);
    auto  files = dir.entryList(QStringList() << "*.mjs" << "*.js", QDir::Files);
    Game* game = Game::get();

    for (auto scriptPath : files)
    {
      auto script = game->loadScript(path + '/' + scriptPath);

      if (!script.isBool())
      {
        TYPE plugin;

        plugin.name = scriptPath.split('/').last().replace(".mjs", "").replace(".js", "");
        plugin.script = script;
        results.insert(plugin.name, plugin);
      }
    }
    return results;
  }
};

#endif // PLUGINLOADER_H
