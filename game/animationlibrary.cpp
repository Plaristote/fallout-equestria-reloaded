#include "globals.h"
#include "animationlibrary.h"
#include "utils/layeredspritesheet.h"
#include <QRegularExpression>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

QmlSpriteAnimation::QmlSpriteAnimation(QObject* parent) : QObject(parent)
{
  connect(this, &QmlSpriteAnimation::frameCountChanged, this, &QmlSpriteAnimation::animationChanged);
  connect(this, &QmlSpriteAnimation::frameIntervalChanged, this, &QmlSpriteAnimation::animationChanged);
  connect(this, &QmlSpriteAnimation::firstFramePositionChanged, this, &QmlSpriteAnimation::animationChanged);
  connect(this, &QmlSpriteAnimation::sourceChanged, this, &QmlSpriteAnimation::animationChanged);
  connect(this, &QmlSpriteAnimation::nameChanged, this, &QmlSpriteAnimation::animationChanged);
  connect(this, &QmlSpriteAnimation::repeatChanged, this, &QmlSpriteAnimation::animationChanged);
  connect(this, &QmlSpriteAnimation::clippedRectChanged, this, &QmlSpriteAnimation::animationChanged);
}

void QmlSpriteAnimation::initialize(const QString& group, const QString& name)
{
  this->group = group;
  this->oldName = name;
  SpriteAnimation::operator=(AnimationLibrary::get()->getAnimation(group, name));
  emit frameIntervalChanged();
  emit frameCountChanged();
  emit firstFramePositionChanged();
  emit sourceChanged();
  emit nameChanged();
  emit repeatChanged();
  emit clippedRectChanged();
}

bool QmlSpriteAnimation::hasChanged() const
{
  const SpriteAnimation& self = static_cast<const SpriteAnimation&>(*this);
  const SpriteAnimation other = AnimationLibrary::get()->getAnimation(group, oldName);

  return self.name                     != other.name ||
         self.repeat                   != other.repeat ||
         toRelativeSource(self.source) != toRelativeSource(other.source) ||
         self.frameInterval            != other.frameInterval ||
         self.frameCount               != other.frameCount ||
         self.firstFramePosition       != other.firstFramePosition ||
         self.clippedRect.size()       != other.clippedRect.size();
}

QString QmlSpriteAnimation::getRelativeSource() const
{
  return toRelativeSource(source);
}

QString QmlSpriteAnimation::toRelativeSource(const QString& source)
{
  return QString(source).replace(ASSETS_PATH + "sprites/", "");
}

AnimationLibrary* AnimationLibrary::self = nullptr;

AnimationLibrary::AnimationLibrary(QObject *parent) : QObject(parent)
{
  self = this;
}

AnimationLibrary::~AnimationLibrary()
{
  self = nullptr;
}

void AnimationLibrary::initialize()
{
  QFile sourceFile(ASSETS_PATH + "/sprites.json");

  if (sourceFile.open(QIODevice::ReadOnly))
  {
    data = QJsonDocument::fromJson(sourceFile.readAll()).object();

    sourceFile.close();
    for (const QString& groupName : data.keys())
    {
      QJsonObject groupObject = data[groupName].toObject();
      QString defaultTexture = groupObject["defaultSource"].toString();

      if (!defaultTexture.isEmpty() && textures.indexOf(defaultTexture) < 0)
        textures << defaultTexture;
      for (const QString& animationName : groupObject.keys())
      {
        QString source = groupObject[animationName].toObject()["source"].toString();

        if (!source.isEmpty() && textures.indexOf(source) < 0)
          textures << source;
      }
    }
    for (const auto& texture : textures)
      images.insert(ASSETS_PATH + "sprites/" + texture, QImage(ASSETS_PATH + "sprites/" + texture));
    emit initialized();
  }
  else
    qDebug() << "!! Could not load sprites.json";
}

const QImage& AnimationLibrary::getImage(const QString& group, const QString& animation) const
{
  return getImage(getAnimation(group, animation).source);
}

const QImage& AnimationLibrary::getImage(const QString& source) const
{
  const auto iterator = images.constFind(source);

  if (iterator == images.end())
  {
    qDebug() << "Could not find Image for picture" << source << "in" << images.keys();
    throw std::out_of_range("no source for animation");
  }
  return *iterator;
}

static SpriteAnimation makeDefaultSpriteAnimation(const QString& animation, const QString& defaultSource)
{
  SpriteAnimation object;

  object.name = animation;
  if (defaultSource.length() > 0)
    object.source = ASSETS_PATH + "sprites/" + defaultSource;
  object.repeat = false;
  object.frameCount = 1;
  object.frameInterval = 0;
  object.firstFramePosition = QPoint(0, 0);
  object.clippedRect = QRect(0, 0, 0, 0);
  return object;
}

SpriteAnimation AnimationLibrary::getAnimation(const QString &group, const QString &animation) const
{
  SpriteAnimation object;
  auto groupData = data[group];
  auto animationData = groupData[animation];
  auto defaultSource = groupData["defaultSource"].toString();

  if (!groupData["cloneOf"].isUndefined())
  {
    defaultSource = groupData["defaultSource"].toString();
    groupData     = data[groupData["cloneOf"].toString()].toObject();
    animationData = groupData[animation];
  }
  if (animationData.isObject())
  {
    QString relativeSource = animationData["source"].toString();

    object.name          = animation;
    object.source        = ASSETS_PATH + "sprites/";
    object.source       += relativeSource.length() > 0 ? relativeSource : defaultSource;
    object.repeat        = animationData["repeat"].toBool(false);
    object.frameCount    = animationData["frameCount"].toInt(1);
    object.frameInterval = animationData["frameInterval"].toInt(100);
    object.firstFramePosition.setX(animationData["offsetX"].toInt(0));
    object.firstFramePosition.setY(animationData["offsetY"].toInt(0));
    object.clippedRect.setX(object.firstFramePosition.x());
    object.clippedRect.setY(object.firstFramePosition.y());
    object.clippedRect.setWidth(animationData["width"].toInt());
    object.clippedRect.setHeight(animationData["height"].toInt());
  }
  else if (groupData["idle"].isObject())
    return getAnimation(group, "idle");
  else
    return makeDefaultSpriteAnimation(animation, defaultSource);
  return object;
}

QString AnimationLibrary::getDefaultSource(const QString& group) const
{
  auto groupData = data[group].toObject();

  return groupData["defaultSource"].toString();
}

void AnimationLibrary::setDefaultSource(const QString& group, const QString& defaultSource)
{
  auto groupData = data[group].toObject();

  groupData.insert("defaultSource", defaultSource);
  data[group] = groupData;
}

void AnimationLibrary::setAnimation(const QString& group, const QString& name, QmlSpriteAnimation* animation)
{
  if (name.length() > 0)
  {
    auto groupData = data[group].toObject();
    QString defaultSource = groupData["defaultSource"].toString();

    if (!groupData["cloneOf"].isString())
      setAnimationWithDefaultSource(group, name, animation, defaultSource);
    else
      setAnimationWithDefaultSource(groupData["cloneOf"].toString(), name, animation, defaultSource);
  }
}

void AnimationLibrary::setAnimationWithDefaultSource(const QString &group, const QString &name, QmlSpriteAnimation* animation, const QString &defaultSource)
{
  if (name.length() > 0)
  {
    auto groupData = data[group].toObject();

    if (!groupData["cloneOf"].isString()) {
      QJsonObject animationData;
      QString     source = animation->getRelativeSource();

      if (source == defaultSource || source.length() == 0)
        animationData.remove("source");
      else
        animationData["source"] = source;
      animationData["repeat"] = animation->repeat;
      animationData["frameCount"] = animation->frameCount;
      animationData["frameInterval"] = animation->frameInterval;
      animationData["offsetX"] = animation->firstFramePosition.x();
      animationData["offsetY"] = animation->firstFramePosition.y();
      animationData["width"] = animation->clippedRect.width();
      animationData["height"] = animation->clippedRect.height();
      groupData.remove(name);
      groupData.insert(animation->name, animationData);
      groupData.remove("cloneOf");
      data[group] = groupData;
    }
    else
      setAnimationWithDefaultSource(groupData["cloneOf"].toString(), name, animation, defaultSource);
  }

}

void AnimationLibrary::save()
{
  QFile sourceFile(ASSETS_PATH + "sprites.json");

  if (sourceFile.open(QIODevice::WriteOnly))
  {
    QJsonDocument document(data);
    sourceFile.write(document.toJson());
    sourceFile.close();
  }
}

void AnimationLibrary::remove(const QString &group, const QString &name)
{
  auto groupData = data[group].toObject();
  int minKeys = groupData.contains("defaultSource") ? 1 : 0;

  groupData.remove(name);
  if (groupData.keys().length() > minKeys)
    data[group] = groupData;
  else
    data.remove(group);
  save();
}

QStringList AnimationLibrary::getGroups() const
{
  return data.keys();
}

QStringList AnimationLibrary::getAnimationList(const QString& group) const
{
  auto groupData = data[group].toObject();
  auto list = groupData.keys();

  list.removeAll("defaultSource");
  if (list.contains("cloneOf"))
    return getAnimationList(groupData["cloneOf"].toString());
  return list;
}

void registerPrerenderedSpritesheet(const QString& baseName, const QString& cloneOf)
{
  const QDir    armors(ASSETS_PATH + "sprites/armors");
  const QString prerenderPath(".prerender/spritesheets");

  for (const QString& armorSource : armors.entryList())
  {
    const QString armorName = QString(armorSource).replace(QRegularExpression("\\..{3,4}$"), "");
    const QString finalName = baseName + '-' + armorName;

    QJsonObject spriteData;

    spriteData["defaultSource"] = prerenderPath + '/' + finalName + ".png";
    spriteData["cloneOf"] = cloneOf;

  }
}

const QString AnimationLibrary::prerenderPath = QString(".prerender/spritesheets");

QString AnimationLibrary::getCharacterSpriteName(const CharacterSpriteDescriptor& descriptor) const
{
  QStringList parts;

  parts << descriptor.base << descriptor.bodyColor.name().replace('#', "")
        << descriptor.hair << descriptor.hairColor.name().replace('#', "");
  parts << "static"  << (descriptor.baseStaticColor.isEmpty() ? "blank" : descriptor.baseStaticColor);
  parts << "overlay" << (descriptor.overLayer.isEmpty()       ? "blank" : descriptor.overLayer);
  parts << "armor"   << (descriptor.armor.isEmpty()           ? "blank" : descriptor.armor);
  parts << "equip"   << (descriptor.weapon.isEmpty()          ? "blank" : descriptor.weapon);
  return parts.join('-');
}

QString AnimationLibrary::getCharacterSpriteFilepath(const CharacterSpriteDescriptor& descriptor) const
{
  return prerenderPath + '/' + getCharacterSpriteName(descriptor) + '.' + getCharacterSpriteFormat();
}

bool AnimationLibrary::hasSpriteSheetBeenPreRendered(const CharacterSpriteDescriptor& descriptor) const
{
  return QFile::exists(getCharacterSpriteFilepath(descriptor));
}

void AnimationLibrary::prerenderCharacterSpriteSheet(const CharacterSpriteDescriptor& descriptor)
{
  const QImage       baseImage(ASSETS_PATH + "sprites/characters/" + descriptor.base + ".png");
  const QString      shadowPath(ASSETS_PATH + "sprites/characters/" + descriptor.base + "-shadow.png");
  LayeredSpriteSheet spritesheet(baseImage.size());
  const QMap<QString, QString> layers({
    {"sprites/characters/armors",  descriptor.armor},
    {"sprites/characters/weapons", descriptor.weapon}
  });

  if (QFile::exists(shadowPath))
    spritesheet.addLayer(QImage(shadowPath));
  spritesheet.addLayer(baseImage);
  spritesheet.addColorLayer(descriptor.bodyColor, baseImage);
  if (descriptor.baseStaticColor.length() > 0)
    spritesheet.addLayer(QImage(ASSETS_PATH + "sprites/characters/" + descriptor.baseStaticColor + ".png"));
  for (auto it = layers.begin() ; it != layers.end() ; ++it)
  {
    const QImage image(ASSETS_PATH + it.key() + '/' + descriptor.base + '/' + it.value());

    spritesheet.addLayer(image);
  }
  if (descriptor.hair.length() > 0)
  {
    QImage hairImage(ASSETS_PATH + "sprites/characters/hairstyles/" + descriptor.hair + ".png");

    spritesheet.addLayer(hairImage);
    spritesheet.addColorLayer(descriptor.hairColor, hairImage);
  }
  if (descriptor.overLayer.length() > 0)
  {
    QImage overLayer(ASSETS_PATH + "sprites/characters/" + descriptor.overLayer + ".png");

    spritesheet.addLayer(overLayer);
    spritesheet.addColorLayer(descriptor.bodyColor, overLayer);
  }
  QDir::current().mkpath(prerenderPath);
  spritesheet.save(getCharacterSpriteFilepath(descriptor));
}

void AnimationLibrary::registerCharacterSpriteSheet(const CharacterSpriteDescriptor& descriptor)
{
  QString name = getCharacterSpriteName(descriptor);

  if (!data[name].isObject())
  {
    QString filePath = getCharacterSpriteFilepath(descriptor);
    QJsonObject spriteData;

    if (!hasSpriteSheetBeenPreRendered(descriptor))
      prerenderCharacterSpriteSheet(descriptor);
    spriteData["defaultSource"] = "../../" + getCharacterSpriteFilepath(descriptor);
    spriteData["cloneOf"]       = descriptor.cloneOf;
    data[name] = spriteData;
    textures << spriteData["defaultSource"].toString();
    images.insert(ASSETS_PATH + "sprites/" + spriteData["defaultSource"].toString(), QImage(filePath));
  }
}
