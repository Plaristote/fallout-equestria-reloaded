#include "wallvector.h"
#include <QPainter>
#include <QDebug>

void renderWallGroup(WallGroup& wallGroup)
{
  // Vérifier que le groupe de murs n'est pas vide
  if (wallGroup.wallTiles.isEmpty()) {
    return;
  }

  // Dimensions des tuiles de base
  const int TILE_WIDTH = 72;
  const int TILE_HEIGHT = 36;

  // Trouver les dimensions maximales de texture
  int maxTextureWidth = 0;
  int maxTextureHeight = 0;
  for (Tile* tile : wallGroup.wallTiles)
  {
    QRect sourceRect = tile->getRect();
    maxTextureWidth = qMax(maxTextureWidth, sourceRect.width());
    maxTextureHeight = qMax(maxTextureHeight, sourceRect.height());
  }

  // Calculer les dimensions du rendu
  int renderWidth = TILE_WIDTH / 2 * (wallGroup.wallTiles.size() + 1);
  int renderHeight = wallGroup.wallTiles.first()->getRect().height() + TILE_HEIGHT / 2 * (wallGroup.wallTiles.size() - 1);

  // Créer une image transparente pour le rendu
  wallGroup.image = QImage(renderWidth, renderHeight, QImage::Format_ARGB32_Premultiplied);
  wallGroup.image.fill(Qt::transparent);

  // Créer un painter pour dessiner sur l'image
  QPainter painter(&wallGroup.image);

  // Coordonnées de rendu actuelles
  int currentX = 0;
  int currentY = 0;

  // Parcourir chaque tile de mur
  for (Tile* tile : wallGroup.wallTiles)
  {
    // Récupérer la texture source et son rectangle
    QImage sourceTexture = tile->getTexture();
    QRect sourceRect = tile->getRect();

    // Dessiner la texture à la position courante
    painter.drawImage(
      QRect(currentX, currentY, sourceRect.width(), sourceRect.height()),
      sourceTexture,
      sourceRect
    );

    // Mettre à jour les coordonnées pour le prochain tile
    currentX += TILE_WIDTH / 2;
    currentY += TILE_HEIGHT / 2;
  }

  // Terminer le dessin
  painter.end();

  // Sauvegarder l'image si un chemin de pré-rendu est fourni
  if (!wallGroup.prerenderPath.isEmpty())
  {
    wallGroup.image.save(wallGroup.prerenderPath);
    qDebug() << "Rendered walls at" << wallGroup.prerenderPath;
  }
}
