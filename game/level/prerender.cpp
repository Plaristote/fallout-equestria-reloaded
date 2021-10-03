#include "prerender.h"
#include "tilemap/tilelayer.h"
#include "tilemap/tilemap.h"
#include <QDir>

PreRenderComponent::PreRenderComponent(QObject* parent) : ParentType(parent)
{
}

void PreRenderComponent::load(const QJsonObject& data)
{
  QDir dir;
  QList<TileLayer*> layers;

  ParentType::load(data);
  for (LevelGrid* grid : getFloors())
  {
    layers << grid->getTilemap()->getLayer("ground")
           << grid->getTilemap()->getRoofs()
           << grid->getTilemap()->getLights();
  }
  if (!dir.exists(getPreRenderPath()))
    preRenderAllTilemaps();
  for (TileLayer* layer : layers)
    layer->setProperty("prerendered", true);
}

QString PreRenderComponent::getPreRenderPath() const
{
  return QDir::currentPath() + "/.prerender/" + name + '/';
}

void PreRenderComponent::preRenderAllTilemaps()
{
  unsigned int i = 0;

  for (LevelGrid* grid : getFloors())
  {
    preRenderTilemap(grid->getTilemap(), "floor" + QString::number(i));
    ++i;
  }
}

void PreRenderComponent::preRenderTilemap(TileMap* tilemap, const QString& prefix)
{
  QDir dir;

  dir.mkpath(getPreRenderPath());
  preRenderGround(tilemap, prefix);
  preRenderLayers(tilemap->getRoofs(), prefix + "_roof");
  preRenderLayers(tilemap->getLights(), prefix + "_lights");
}

void PreRenderComponent::preRenderGround(TileMap* tilemap, const QString& prefix)
{
  QString     tilemapFile = getPreRenderPath() + prefix + "_tilemap.png";
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
