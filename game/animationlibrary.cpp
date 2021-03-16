#include "globals.h"
#include "animationlibrary.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

void QmlSpriteAnimation::initialize(const QString &group, const QString &name)
{
  SpriteAnimation::operator=(AnimationLibrary::get()->getAnimation(group, name));
  emit frameIntervalChanged();
  emit frameCountChanged();
  emit firstFramePositionChanged();
  emit sourceChanged();
  emit nameChanged();
  emit repeatChanged();
  emit clippedRectChanged();
}

QString QmlSpriteAnimation::getRelativeSource() const
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
    emit initialized();
  }
  else
    qDebug() << "!! Could not load sprites.json";
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
  object.name          = animation;
  object.source        = ASSETS_PATH + "sprites/";
  object.source       += animationData["source"].toString(defaultSource);
  object.repeat        = animationData["repeat"].toBool(false);
  object.frameCount    = animationData["frameCount"].toInt(1);
  object.frameInterval = animationData["frameInterval"].toInt(100);
  object.firstFramePosition.setX(animationData["offsetX"].toInt(0));
  object.firstFramePosition.setY(animationData["offsetY"].toInt(0));
  object.clippedRect.setX(object.firstFramePosition.x());
  object.clippedRect.setY(object.firstFramePosition.y());
  object.clippedRect.setWidth(animationData["width"].toInt());
  object.clippedRect.setHeight(animationData["height"].toInt());
  return object;
}

void AnimationLibrary::setAnimation(const QString& group, const QString& name, QmlSpriteAnimation* animation)
{
  auto groupData = data[group].toObject();
  QJsonObject animationData;

  animationData["source"] = animation->getRelativeSource();
  animationData["repeat"] = animation->repeat;
  animationData["frameCount"] = animation->frameCount;
  animationData["frameInterval"] = animation->frameInterval;
  animationData["offsetX"] = animation->firstFramePosition.x();
  animationData["offsetY"] = animation->firstFramePosition.y();
  animationData["width"] = animation->clippedRect.width();
  animationData["height"] = animation->clippedRect.height();
  groupData.remove(name);
  groupData.insert(animation->name, animationData);
  data[group] = groupData;
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

QStringList AnimationLibrary::getGroups() const
{
  return data.keys();
}

QStringList AnimationLibrary::getAnimationList(const QString& group) const
{
  auto list = data[group].toObject().keys();

  list.removeAll("defaultSource");
  return list;
}
