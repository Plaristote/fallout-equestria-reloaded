#include "ambientlightcomponent.h"
#include "game.h"
#include <QJsonArray>

QColor     jsonToColor(QJsonValue value);
QJsonArray colorToJson(QColor color);

static QMap<int, QColor> daylightColors = {
  {0,  QColor("#89061862")},
  {6,  QColor("#634b341e")},
  {8,  QColor("#00ffffff")},
  {20, QColor("#3958c8dc")},
  {22, QColor("#52082185")}
};

AmbientLightComponent::AmbientLightComponent(QObject *parent) : ParentType(parent)
{
  ambientColor = QColor(255, 255, 255, 0.0);
}

void AmbientLightComponent::update(qint64 delta)
{
  if (usesDaylight())
    updateDaylight();
  ParentType::update(delta);
}

void AmbientLightComponent::updateDaylight()
{
  TimeManager* timeManager = Game::get()->getTimeManager();
  QColor       newColor;

  for (auto it = daylightColors.begin() ; it != daylightColors.end() ; ++it)
  {
    if (it.key() <= timeManager->getHour())
      newColor = it.value();
  }
  if (ambientColor != newColor)
  {
    ambientColor = newColor;
    emit ambientColorChanged();
  }
}

void AmbientLightComponent::load(const QJsonObject& data)
{
  ParentType::load(data);
  useAmbientLight = data["useAmbientLight"].toBool(false);
  useDaylight     = data["useDaylight"].toBool(false);
  ambientColor    = jsonToColor(data["ambientColor"]);
  emit ambientColorChanged();
}

void AmbientLightComponent::save(QJsonObject& data) const
{
  ParentType::save(data);
  data["useAmbientLight"] = useAmbientLight;
  data["useDaylight"]     = useDaylight;
  data["ambientColor"]    = colorToJson(ambientColor);
}
