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
  QList<TileLayer*> layers;

  tilemap->load(name);
  layers << tilemap->getLayer("ground") << tilemap->getRoofs() << tilemap->getLights();
  if (!dir.exists(getPreRenderPath()))
    preRenderTilemap();
  for (auto it = layers.begin() ; it != layers.end() ; ++it)
    (*it)->setProperty("prerendered", true);
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
