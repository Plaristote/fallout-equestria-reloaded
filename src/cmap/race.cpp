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

QVariantMap Race::AgeRange::toVariant() const
{
  return {{"child", child}, {"adult", adult}, {"old", old}, {"lifespan", lifespan}};
}

Race::AgeRange Race::getAgeRange() const
{
  QJSValue source = script.property("ageRange");
  AgeRange ageRange;

  if (source.isObject())
  {
    if (source.hasProperty("child"))
      ageRange.child    = static_cast<unsigned short>(source.property("child").toUInt());
    if (source.hasProperty("adult"))
      ageRange.adult    = static_cast<unsigned short>(source.property("adult").toUInt());
    if (source.hasProperty("lifespan"))
      ageRange.lifespan = static_cast<unsigned short>(source.property("lifespan").toUInt());
    if (source.hasProperty("old"))
      ageRange.old      = static_cast<unsigned short>(source.property("old").toUInt());
    else
      ageRange.old = ageRange.lifespan;
  }
  return ageRange;
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

static CharacterSpriteDescriptor spriteDescriptorFromString(const QString& value)
{
  CharacterSpriteDescriptor descriptor;

  descriptor.layered = false;
  descriptor.base = value;
  return descriptor;
}

static CharacterSpriteDescriptor spriteDescriptorFromObject(QJSValue value)
{
  CharacterSpriteDescriptor descriptor;

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
  return descriptor;
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
    return spriteDescriptorFromString(value.toString());
  else if (value.isObject())
    return spriteDescriptorFromObject(value);
  else
    qDebug() << "Race::getSpriteSheet: No spritesheet defined for race" << name;
  return descriptor;
}

QMap<QString, QString> Race::getItemSlots() const
{
  QMap<QString, QString> map{{"armor", "armor"}, {"use-1","any"},{"use-2","any"}};
  QJSValue               variable = script.property("itemSlots");

  if (!variable.isUndefined())
  {
    QVariantMap variants = variable.toVariant().toMap();

    map.clear();
    for (auto it = variants.begin() ; it != variants.end() ; ++it)
      map.insert(it.key(), it.value().toString());
  }
  return map;
}

QString Race::getDefaultItemForSlot(StatModel* model, const QString& slotName) const
{
  QJSValue hook = script.property("getDefaultItem");

  if (hook.isCallable())
  {
    auto&        scriptEngine = Game::get()->getScriptEngine();
    QJSValueList params{scriptEngine.newQObject(model), slotName};
    QJSValue     retval = hook.call(params);

    if (retval.isString())
      return retval.toString();
  }
  return "melee";
}

void Race::initialize()
{
  races.clear();
  races = CmapPlugins::load<Race>(SCRIPTS_PATH + "cmap/races");
}
