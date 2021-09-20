#include "prerender.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tilemap.h"
#include <QDir>

PreRenderComponent::PreRenderComponent(QObject* parent) : ParentType(parent)
{
}

void PreRenderComponent::load()
{
  if (tilemap->load(name))
  {
    QDir dir;
    QList<TileLayer*> layers;

    layers << tilemap->getLayer("ground") << tilemap->getRoofs() << tilemap->getLights();
    if (!dir.exists(getPreRenderPath()))
      preRenderTilemap();
    for (TileLayer* layer : layers)
      layer->setProperty("prerendered", true);
  }
  ParentType::load();
}

QString PreRenderComponent::getPreRenderPath() const
{
  return QDir::currentPath() + "/.prerender/" + name + '/';
}

void PreRenderComponent::preRenderTilemap()
{
  QDir dir;

  dir.mkpath(getPreRenderPath());
  preRenderGround();
  preRenderLayers(tilemap->getRoofs(), "roof");
  preRenderLayers(tilemap->getLights(), "lights");
}

void PreRenderComponent::preRenderGround()
{
  QString     tilemapFile = getPreRenderPath() + "tilemap.png";
  TileLayer*  ground = tilemap->getLayer("ground");
  QImage      image(ground->getRenderedRect().size(), QImage::Format_ARGB32);
  QPoint      offset(ground->getRenderedRect().x(), 0);
  QStringList nonRenderable({"misc", "walls", "blocks", "ground"});
  const auto& layers = tilemap->getLayers();

  image.fill(Qt::transparent);
  ground->renderToImage(image, offset);
  for (auto it = layers.rbegin() ; it != layers.rend() ; ++it)
  {
    TileLayer* layer = *it;

    if (layer->isVisible() && !nonRenderable.contains(layer->getName()))
      layer->renderToImage(image, offset);
  }
  image.save(tilemapFile);
  ground->setProperty("prerendered", true);
}

void PreRenderComponent::preRenderLayers(const QList<TileLayer*>& layers, const QString &prefix)
{
  for (TileLayer* lightLayer : layers)
  {
    QString fileName = prefix + '_' + lightLayer->getName() + ".png";

    lightLayer->renderToFile(getPreRenderPath() + fileName);
  }
}
