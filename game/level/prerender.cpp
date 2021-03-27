#include "prerender.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tilemap.h"
#include <QDir>

PreRenderComponent::PreRenderComponent(QObject* parent) : ParentType(parent)
{
}

void PreRenderComponent::load()
{
  QDir dir;

  tilemap->load(name);
  if (!dir.exists(getPreRenderPath()))
    preRenderTilemap();
  ParentType::load();
}

QString PreRenderComponent::getPreRenderPath() const
{
  return QDir::currentPath() + "/.prerender/" + name + '/';
}

void PreRenderComponent::preRenderTilemap()
{
  QDir dir;
  QString preRenderFolder = getPreRenderPath();

  dir.mkpath(preRenderFolder);
  tilemap->getLayer("ground")->renderToFile(preRenderFolder + "tilemap.png");
  for (TileLayer* roofLayer : tilemap->getRoofs())
  {
    QString fileName = "roof_" + roofLayer->getName() + ".png";

    roofLayer->renderToFile(preRenderFolder + fileName);
  }
  for (TileLayer* lightLayer : tilemap->getLights())
  {
    QString fileName = "lights_" + lightLayer->getName() + ".png";

    lightLayer->renderToFile(preRenderFolder + fileName);
  }
}
