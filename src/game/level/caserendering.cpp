#include "caserendering.h"
#include "tilemap/tilemap.h"
#include "../leveltask.h"
#include "../../debounce.h"
#include "../character.h"

CaseRendering::CaseRendering(QObject* parent)
  : QObject(parent)
{
}

void CaseRendering::reset()
{
  bool cropHasChanged = false;

  if (!culled)
  {
    culled = true;
    emit culledChanged(culled);
  }
  if (vPlayerCrop)
  {
    vPlayerCrop = false;
    emit vPlayerCropChanged(vPlayerCrop);
  }
  if (hPlayerCrop)
  {
    hPlayerCrop = false;
    emit hPlayerCropChanged(hPlayerCrop);
  }
}

CaseRenderingComponent::CaseRenderingComponent(QObject* parent)
  : ParentType(parent)
{
  auto* player = getPlayer();

  if (player)
  {
    updateCroppedCenterFunnel = createFunnel([this, player]()
    {
      updateCroppedCenter(player->getPosition(), player->getSpritePosition());
    }, 150);
    connect(player, &Character::spritePositionChanged, this, updateCroppedCenterFunnel);
    connect(player, &Character::positionChanged, this, [this, player]() { updateCroppedWalls(player->getPosition(), player->getSpritePosition()); });
  }
  connect(this, &GridComponent::floorChanged, this, &CaseRenderingComponent::onMapSizeChanged);
}

void CaseRenderingComponent::insertCaseRendering(QPoint position, QObject* value)
{
  int index = position.y() * mapSize.width() + position.x();
  insertCaseRendering(index, value);
}

void CaseRenderingComponent::insertCaseRendering(int index, QObject* value)
{
  CaseRendering* caseRendering = qobject_cast<CaseRendering*>(value);

  if (caseRendering && index < cases.size())
  {
    cases[index] = caseRendering;
    connect(caseRendering, &QObject::destroyed, this, &CaseRenderingComponent::removeCaseRendering);
  }
  else
    qDebug() << "CaseRenderingComponent: invalid call to insertCaseRendering" << index << value;
}

void CaseRenderingComponent::removeCaseRendering(QObject* value)
{
  int index = cases.indexOf(value);
  if (index >= 0)
    cases[index] = nullptr;
}

CaseRendering* CaseRenderingComponent::caseRenderingAt(const QPoint position) const
{
  int index = position.y() * mapSize.width() + position.x();
  return index < cases.size() ? cases[index] : nullptr;
}

QSet<CaseRendering*> CaseRenderingComponent::caseRenderingsIn(const QRect region) const
{
  QSet<CaseRendering*> result;
  const int mapWidth = mapSize.width();

  for (int y = std::max(0, region.top()) ; y < region.bottom() ; ++y)
  {
    const int rowBaseIndex = y * mapWidth;

    for (int x = std::max(0, region.left()) ; x < region.right() ; ++x)
    {
      const int index = rowBaseIndex + x;
      if (cases.size() > index && cases[index] != nullptr)
        result.insert(cases[index]);
    }
  }
  return result;
}

void CaseRenderingComponent::onMapSizeChanged()
{
  TileMap* tileMap = getTileMap();

  if (tileMap)
  {
    mapSize = tileMap->getSize();
    cases.resize(mapSize.width() * mapSize.height(), nullptr);
  }
  for (CaseRendering* caseRendering : cases)
  {
    if (caseRendering)
      caseRendering->reset();
  }
  renderedTileList.clear();
  renderedTileList.reserve(cases.size());
}

void CaseRenderingComponent::updateCulledTiles(const QRect tileRect)
{
  QSet<CaseRendering*> previouslyRendered = renderedTileList;
  const int    mapWidth = mapSize.width();
  const int    startX = qMax(0, tileRect.left());
  const int    startY = qMax(0, tileRect.top());
  const int    endX   = qMin(mapSize.width() - 1, tileRect.right());
  const int    endY   = qMin(mapSize.height() - 1, tileRect.bottom());

  renderedTileList.clear();

  for (int y = startY; y <= endY; ++y)
  {
    const int rowBaseIndex = y * mapWidth;

    for (int x = startX; x <= endX; ++x)
    {
      const int index = rowBaseIndex + x;

      if (index >= 0 && index < cases.size())
      {
        CaseRendering* caseRendering = cases[index];
        caseRendering->updateCulled(false);
        renderedTileList.insert(caseRendering);
      }
    }
  }
  previouslyRendered.subtract(renderedTileList);
  for (CaseRendering* caseRendering : previouslyRendered)
    caseRendering->setCulled(true);
}

static QPoint getCroppedCenter(QObject* wallRenderer, const QPoint spritePosition)
{
  static const QPoint circleOffset(35, 10);
  const QPoint wallOffset = wallRenderer->property("offset").toPoint();

  return QPoint(
    spritePosition.x() - wallOffset.x() + 35,
    spritePosition.y() - wallOffset.y() + 10
  );
}

void CaseRenderingComponent::updateCroppedCenter(const QPoint playerPosition, const QPoint spritePosition)
{
  for (CaseRendering* caseRendering : lastCroppedWalls)
  {
    QPoint position = caseRendering->getPosition();
    QObject* blockRenderer = caseRendering->getBlockRenderer();
    QObject* vWallRenderer = caseRendering->getVWallRenderer();
    QObject* hWallRenderer = caseRendering->getHWallRenderer();
    const char* cropCenterProperty = "cropCircleCenter";

    if (blockRenderer) {
      blockRenderer->setProperty(
        cropCenterProperty, getCroppedCenter(blockRenderer, spritePosition)
      );
    }
    if (hWallRenderer) {
      hWallRenderer->setProperty(
        cropCenterProperty, getCroppedCenter(hWallRenderer, spritePosition)
      );
    }
    if (vWallRenderer) {
      vWallRenderer->setProperty(
        cropCenterProperty, getCroppedCenter(vWallRenderer, spritePosition)
      );
    }
  }
}

void CaseRenderingComponent::updateCroppedWalls(const QPoint playerPosition, const QPoint spritePosition)
{
  if (!LevelTask::withPlayerCropCircle) return ;
  static const char* clippingProperty = "withClipping";
  static const char* cropCenterProperty = "cropCircleCenter";

  const int range = 4;
  const int mapWidth = mapSize.width();
  QSet<CaseRendering*> tilesToUpdate;
  QSet<CaseRendering*> tilesToDisable;
  QRect currentRegion(
    playerPosition.x() - range,
    playerPosition.y() - range,
    range * 2,
    range * 2
  );

  tilesToUpdate = caseRenderingsIn(currentRegion);
  tilesToDisable = lastCroppedWalls;
  tilesToDisable.subtract(tilesToUpdate);
  lastCroppedWalls = tilesToUpdate;

  for (CaseRendering* caseRendering : tilesToUpdate)
  {
    QPoint position = caseRendering->getPosition();
    QObject* blockRenderer = caseRendering->getBlockRenderer();
    QObject* vWallRenderer = caseRendering->getVWallRenderer();
    QObject* hWallRenderer = caseRendering->getHWallRenderer();
    bool blockClipping = position.x() >= playerPosition.x() && position.y() >= playerPosition.y();

    if (blockRenderer)
    {
      blockRenderer->setProperty(clippingProperty, blockClipping);
      blockRenderer->setProperty(cropCenterProperty, getCroppedCenter(blockRenderer, spritePosition));
    }
    if (hWallRenderer)
    {
      hWallRenderer->setProperty(clippingProperty, blockClipping || (position.x() == playerPosition.x() - 1 && position.y() == playerPosition.y()));
      hWallRenderer->setProperty(cropCenterProperty, getCroppedCenter(hWallRenderer, spritePosition));
    }
    if (vWallRenderer)
    {
      vWallRenderer->setProperty(clippingProperty, blockClipping || (position.y() == playerPosition.y() - 1 && position.x() == playerPosition.x()));
      vWallRenderer->setProperty(cropCenterProperty, getCroppedCenter(vWallRenderer, spritePosition));
    }
  }
  for (CaseRendering* caseRendering : tilesToDisable)
  {
    for (QObject* wallRenderer : {caseRendering->getBlockRenderer(), caseRendering->getVWallRenderer(), caseRendering->getHWallRenderer()})
    {
      if (wallRenderer)
        wallRenderer->setProperty(clippingProperty, false);
    }
  }
}
