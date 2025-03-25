#include "wallvector.h"
#include "tilelayer.h"
#include "tilemap.h"

static QList<WallGroup> groupWallTiles(const QList<TileLayer*>& tileLayers);
static unsigned short maxWallLength = 10;

QList<QPoint> WallGroup::positions() const
{
  QList<QPoint> list;

  for (int i = 0 ; i < length ; ++i)
    list << QPoint(startPosition.x() + i, startPosition.y());
  return list;
}

QPoint WallGroup::getRenderPosition() const
{
  if (wallTiles.isEmpty()) return QPoint();

  // Utiliser le dernier tile (coordonnée X la plus élevée)
  Tile* lastTile = wallTiles.last();
  QPoint tilePosition = lastTile->getPosition();
  QSize tileSize(72, 36);  // Taille de tuile définie précédemment

  // Calcul des coordonnées de rendu isométrique
  int renderX = tilePosition.x() * tileSize.width() / 2 - 
                tilePosition.y() * tileSize.width() / 2;
  int renderY = tilePosition.y() * tileSize.height() / 2 + 
                tilePosition.x() * tileSize.height() / 2;

  // Ajustement pour le décalage dû à la longueur du groupe
  renderX -= (length - 1) * tileSize.width() / 2;
  renderY -= (length - 1) * tileSize.height() / 2;

  return QPoint(renderX, renderY);
}

QList<WallGroup> WallGroup::factory(TileMap& tileMap)
{
  TileLayer* hWalls = tileMap.getLayer("walls-h");
  TileLayer* blocks = tileMap.getLayer("blocks");
  QList<TileLayer*> groupables;
  QList<WallGroup> groups;

  if (hWalls)
    groupables << hWalls;
  if (blocks)
    groupables << blocks;
  groups = groupWallTiles(groupables);
  for (auto it = groups.begin() ; it != groups.end() ;)
  {
    if (it->length < 2)
      it = groups.erase(it);
    else
      ++it;
  }
  qDebug() << "Wall Group count:" << groups.size();
  return groups;
}

// Fonction de détection et de regroupement des murs contigus sur l'axe X
static QList<WallGroup> groupWallTiles(const QList<TileLayer*>& tileLayers)
{
  QList<WallGroup> wallGroups;

  // Vérification que toutes les couches ont la même taille
  if (tileLayers.isEmpty()) return wallGroups;
  
  QSize mapSize = tileLayers.first()->getSize();
  
  // Parcours de chaque ligne Y
  for (int y = 0; y < mapSize.height(); ++y)
  {
    // Réinitialisation des variables pour chaque nouvelle ligne
    bool inWallGroup = false;
    WallGroup currentGroup;

    // Parcours de chaque colonne X
    for (int x = 0; x < mapSize.width(); ++x)
    {
      bool isTileWall = false;

      // Vérifier si la tuile est un mur dans n'importe quelle couche
      for (TileLayer* layer : tileLayers)
      {
        Tile* tile = layer->getTile(x, y);

        if (tile != nullptr)
        {
          isTileWall = true;
          currentGroup.wallTiles.append(tile);
          break;  // On considère qu'un mur dans une couche suffit
        }
      }

      // Logique de groupement
      if (isTileWall)
      {
        if (!inWallGroup)
        {
          // Début d'un nouveau groupe de murs
          inWallGroup = true;
          currentGroup.startPosition = QPoint(x, y);
          currentGroup.length = 1;
        }
        else
        {
          // Extension du groupe de murs
          currentGroup.length++;
          if (currentGroup.length > maxWallLength)
          {
            wallGroups.append(currentGroup);
            currentGroup = WallGroup();
            inWallGroup = false;
          }
        }
      }
      else
      {
        // Fin d'un groupe de murs
        if (inWallGroup)
        {
          wallGroups.append(currentGroup);
          currentGroup = WallGroup();  // Réinitialisation
          inWallGroup = false;
        }
      }
    }

    // Gestion du cas où un groupe de murs termine à la fin de la ligne
    if (inWallGroup)
    {
      wallGroups.append(currentGroup);
    }
  }
  return wallGroups;
}
