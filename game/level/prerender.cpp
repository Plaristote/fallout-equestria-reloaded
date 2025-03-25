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
    TileLayer* ground = grid->getTilemap()->getLayer("ground");

    if (ground)
      layers << ground;
    layers << grid->getTilemap()->getRoofs()
           << grid->getTilemap()->getLights();
  }
#ifdef GAME_EDITOR
  clearCache();
#endif
  if (!dir.exists(getPreRenderPath()))
    preRenderAllTilemaps();
  for (TileLayer* layer : layers)
    layer->setProperty("prerendered", true);
}

QString PreRenderComponent::getPreRenderPath() const
{
  return QDir::currentPath() + "/.prerender/" + name + '/';
}

bool PreRenderComponent::clearCache() const
{
  QDir directory(getPreRenderPath());

  return directory.removeRecursively();
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
  preRenderWallVectors(tilemap, prefix + "_wall");
}

void renderWallGroup(WallGroup&);

void PreRenderComponent::preRenderWallVectors(TileMap* tilemap, const QString& prefix)
{
  return prepareWallVectors(tilemap, prefix, true);
}

void PreRenderComponent::prepareWallVectors()
{
  unsigned int i = 0;

  for (LevelGrid* grid : getFloors())
  {
    prepareWallVectors(grid->getTilemap(), "floor" + QString::number(i) + "_wall");
    ++i;
  }
}

void PreRenderComponent::prepareWallVectors(TileMap* tilemap, const QString& prefix, bool render)
{
  TileLayer* blocks = tilemap->getLayer("blocks");
  TileLayer* hWalls = tilemap->getLayer("walls-h");

  for (WallGroup& wallGroup : tilemap->getWallGroups())
  {
    QPoint position = wallGroup.endPosition();
    Tile* tile = wallGroup.wallTiles.last();
    QPoint renderPosition = tile->getRenderPosition();

    wallGroup.prerenderPath = getPreRenderPath() + prefix + '_' + QString::number(position.x()) + '_' + QString::number(position.y()) + ".png";
    for (QPoint position : wallGroup.positions())
    {
      for (TileLayer* layer : QList<TileLayer*>{blocks, hWalls})
      {
        if (layer)
        {
          Tile* positionTile = layer->getTile(position.x(), position.y());
          if (positionTile != tile)
            layer->setTileIdAt(position.x(), position.y(), nullptr, 0);
        }
      }
    }

    if (render)
      renderWallGroup(wallGroup);
    else
      wallGroup.image = QImage(wallGroup.prerenderPath);

    renderPosition.setX(renderPosition.x() - (wallGroup.image.width() * 2  - tile->getRect().width()));
    renderPosition.setY(renderPosition.y() - (wallGroup.image.height() * 2 - tile->getRect().height()));
    tile->setImage(&wallGroup.prerenderPath);
    tile->setTexture(&wallGroup.image);
    tile->setRect(wallGroup.image.rect());
    tile->setRenderPosition(renderPosition);
  }
}

void PreRenderComponent::preRenderGround(TileMap* tilemap, const QString& prefix)
{
  QString     tilemapFile = getPreRenderPath() + prefix + "_tilemap.png";
  TileLayer*  ground = tilemap->getLayer("ground");
  QImage      image(ground->getRenderedRect().size(), QImage::Format_ARGB32);
  QPoint      offset(ground->getRenderedRect().x(), 0);
  QStringList nonRenderable({"misc", "walls-v", "walls-h", "blocks", "ground"});
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
