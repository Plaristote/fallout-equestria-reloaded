#include "animationlibrary.h"
#include <QFile>
#include <QJsonObject>
#include <QDebug>

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
  QFile sourceFile(":/assets/sprites.json");

  if (sourceFile.open(QIODevice::ReadOnly))
  {
    data = QJsonDocument::fromJson(sourceFile.readAll());

    for (const QString& groupName : data.object().keys())
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

  object.source        = "assets/sprites/";
  object.source       += animationData["source"].toString(groupData["defaultSource"].toString());
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
