#include "race.h"
#include "game.h"
#include "pluginloader.h"
#include <QDebug>
#include <QDir>

QMap<QString, Race> Race::races;

bool Race::isPlayable() const
{
  if (script.hasProperty("isPlayable"))
    return script.property("isPlayable").toBool();
  return false;
}

bool Race::withFaceColor() const
{
  if (script.hasProperty("withFaceColor"))
    return script.property("withFaceColor").toBool();
  return false;
}

QStringList Race::getFaces() const
{
  if (script.hasProperty("faces"))
    return script.property("faces").toVariant().toStringList();
  return QStringList();
}

QStringList Race::getHairs(QString face) const
{
  if (face.isEmpty() && getFaces().size() > 0)
    face = *(getFaces().begin());
  if (!face.isEmpty())
  {
    QString     path = ASSETS_PATH + "faces/" + face + "/hairstyles";
    QStringList list = QDir(path).entryList(QDir::Files);

    for (QString& entry : list)
      entry = entry.replace(".png", "");
    list.prepend("");
    return list;
  }
  return QStringList() << "";
}

CharacterSpriteDescriptor Race::getSpriteSheet(StatModel* model) const
{
  CharacterSpriteDescriptor descriptor;
  QJSValue value = script.property("spriteSheet");

  if (value.isCallable())
  {
    auto& scriptEngine = Game::get()->getScriptEngine();

    value = value.call(QJSValueList() << scriptEngine.newQObject(model));
  }
  if (value.isString())
  {
    descriptor.layered = false;
    descriptor.base = value.toString();
  }
  else if (value.isObject())
  {
    descriptor.layered = true;
    descriptor.base = value.property("base").toString();
    descriptor.cloneOf = value.property("cloneOf").toString();
    if (value.hasProperty("staticBase"))
      descriptor.baseStaticColor = value.property("staticBase").toString();
    if (value.hasProperty("hair") && value.property("hair").isString())
       descriptor.hair = value.property("hair").toString();
    if (value.hasProperty("overlay") && value.property("overlay").isString())
      descriptor.overLayer = value.property("overlay").toString();
    if (value.hasProperty("color"))
      descriptor.bodyColor = QColor(value.property("color").toString());
    else
      descriptor.bodyColor = Qt::transparent;
    if (value.hasProperty("hairColor"))
      descriptor.hairColor = QColor(value.property("hairColor").toString());
    else
      descriptor.hairColor = Qt::transparent;
  }
  else
    qDebug() << "Race::getSpriteSheet: No spritesheet defined for race" << name;
  return descriptor;
}

void Race::initialize()
{
  races.clear();
  races = CmapPlugins::load<Race>(SCRIPTS_PATH + "cmap/races");
}
