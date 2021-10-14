#include "gridobjectcomponent.h"

GridObjectComponent::GridObjectComponent(QObject *parent) : ScriptableComponent(parent)
{
  floor = 0;
}

void GridObjectComponent::load(const QJsonObject& data)
{
  position.setX(data["x"].toInt(0));
  position.setY(data["y"].toInt(0));
  floor = static_cast<unsigned char>(data["z"].toInt(0));
  cover = static_cast<char>(data["cover"].toInt(100));
  ScriptableComponent::load(data);
}

void GridObjectComponent::save(QJsonObject& data) const
{
  ScriptableComponent::save(data);
  if (position != QPoint(0,0))
  {
    data["x"] = position.x();
    data["y"] = position.y();
  }
  if (floor != 0)
    data["z"] = QJsonValue::fromVariant(static_cast<unsigned int>(floor));
  if (cover != 100)
    data["cover"] = static_cast<int>(cover);
}
