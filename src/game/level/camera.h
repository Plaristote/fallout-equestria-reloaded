#ifndef  CAMERACOMPONENT_H
# define CAMERACOMPONENT_H

# include "caserendering.h"

class CameraComponent : public CaseRenderingComponent
{
  Q_OBJECT
  typedef CaseRenderingComponent ParentType;

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
  void renderedTilesChanged(QRect);

private slots:
  void updateRenderedTiles();

private:
  QPoint offset;
  QSize  size;
  QTimer updateTimer;
  QRect renderedTiles;
};

#endif
