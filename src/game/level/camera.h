#ifndef  CAMERACOMPONENT_H
# define CAMERACOMPONENT_H

# include "prerender.h"

class CameraComponent : public PreRenderComponent
{
  Q_OBJECT
  typedef PreRenderComponent ParentType;

  Q_PROPERTY(QPoint canvasOffset MEMBER offset NOTIFY cameraMoved)
  Q_PROPERTY(QSize  canvasSize   MEMBER size   NOTIFY cameraSizeChanged)
  Q_PROPERTY(QRect  renderedTiles MEMBER renderedTiles NOTIFY renderedTilesChanged)
public:
  explicit CameraComponent(QObject* parent = nullptr);

  QRect getRenderedTiles() const;
  const QPoint& cameraOffset() const { return offset; }

  Q_INVOKABLE bool isCaseRendered(int x, int y) const { return renderedTiles.contains(x, y); }

signals:
  void cameraMoved();
  void cameraSizeChanged();
  void renderedTilesChanged();

private slots:
  void updateRenderedTiles();

private:
  QPoint offset;
  QSize  size;
  QTimer updateTimer;
  QRect renderedTiles;
};

#endif
