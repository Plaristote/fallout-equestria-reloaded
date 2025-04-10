#ifndef  CASERENDERING_H
# define CASERENDERING_H

# include "prerender.h"

class CaseRendering : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QPoint   position     MEMBER position NOTIFY positionChanged)
  Q_PROPERTY(bool     culled       MEMBER culled NOTIFY culledChanged)
  Q_PROPERTY(QObject* blockRenderer READ getBlockRenderer WRITE setBlockRenderer NOTIFY blockRendererChanged)
  Q_PROPERTY(QObject* vWallRenderer READ getVWallRenderer WRITE setVWallRenderer NOTIFY vWallRendererChanged)
  Q_PROPERTY(QObject* hWallRenderer READ getHWallRenderer WRITE setHWallRenderer NOTIFY hWallRendererChanged)
  Q_PROPERTY(bool     vPlayerCrop   MEMBER vPlayerCrop NOTIFY vPlayerCropChanged)
  Q_PROPERTY(bool     hPlayerCrop   MEMBER hPlayerCrop NOTIFY hPlayerCropChanged)
public:
  explicit CaseRendering(QObject* parent = nullptr);

  void reset();
  QPoint getPosition() const { return position; }
  bool isCulled() const { return culled; }
  void setCulled(bool value) { culled = value; emit culledChanged(value); }
  void updateCulled(bool value) { if (value != culled) setCulled(value); }

#define RENDERER_METHODS(TITLE, VARNAME) \
  void set##TITLE##Renderer(QObject* value) \
  { \
    VARNAME = value; \
    connect(VARNAME, &QObject::destroyed, this, &CaseRendering::unset##TITLE##Renderer); \
    emit VARNAME##Changed(); \
  } \
  QObject* get##TITLE##Renderer() const \
  { \
    return VARNAME; \
  } \
  void unset##TITLE##Renderer(QObject* value) \
  { \
    if (VARNAME == value) \
    { \
      VARNAME = nullptr; \
      emit VARNAME##Changed(); \
    } \
  }

  RENDERER_METHODS(Block, blockRenderer)
  RENDERER_METHODS(VWall, vWallRenderer)
  RENDERER_METHODS(HWall, hWallRenderer)
#undef RENDERER_METHODS

  bool hasVPlayerCrop() const { return vPlayerCrop; }
  bool hasHPlayerCrop() const { return hPlayerCrop; }
  void setVPlayerCrop(bool value) { vPlayerCrop = value; emit vPlayerCropChanged(value); }
  void setHPlayerCrop(bool value) { hPlayerCrop = value; emit hPlayerCropChanged(value); }
  void updateVPlayerCrop(bool value) { if (value != vPlayerCrop) setVPlayerCrop(value); }
  void updateHPlayerCrop(bool value) { if (value != hPlayerCrop) setHPlayerCrop(value); }

signals:
  void positionChanged(QPoint);
  void culledChanged(bool);
  void blockRendererChanged();
  void vWallRendererChanged();
  void hWallRendererChanged();
  void vPlayerCropChanged(bool);
  void hPlayerCropChanged(bool);

private:
  QPoint   position;
  bool     culled = true;
  QObject* blockRenderer = nullptr;
  QObject* vWallRenderer = nullptr;
  QObject* hWallRenderer = nullptr;
  bool     vPlayerCrop = false;
  bool     hPlayerCrop = false;
};

class CaseRenderingComponent : public PreRenderComponent
{
  Q_OBJECT
  typedef PreRenderComponent ParentType;
public:
  explicit CaseRenderingComponent(QObject* parent = nullptr);

public slots:
  void insertCaseRendering(QPoint position, QObject* value);
  void insertCaseRendering(int index, QObject* value);
  void removeCaseRendering(QObject* value);

protected slots:
  void onMapSizeChanged();
  void updateCulledTiles(const QRect renderedTiles);
  void updateCroppedWalls(const QPoint playerPosition, const QPoint spritePosition);
  void updateCroppedCenter(const QPoint playerPosition, const QPoint spritePosition);

private:
  CaseRendering* caseRenderingAt(const QPoint) const;
  QSet<CaseRendering*> caseRenderingsIn(const QRect) const;

  QList<CaseRendering*> cases;
  QSize mapSize;
  QSet<CaseRendering*> renderedTileList;
  QSet<CaseRendering*> lastCroppedWalls;
  std::function<void()> updateCroppedCenterFunnel;
};

#endif
