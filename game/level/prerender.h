#ifndef  PRERENDERCOMPONENT_H
# define PRERENDERCOMPONENT_H

# include "zone.h"

class PreRenderComponent : public ZoneComponent
{
  Q_OBJECT
  typedef ZoneComponent ParentType;

  Q_PROPERTY(QString preRenderPath READ getPreRenderPath CONSTANT)
public:
  PreRenderComponent(QObject* parent = nullptr);

  void load(const QJsonObject&);

protected:
  virtual bool usePrerenderCache() const { return true; }

private:
  bool    clearCache() const;
  void    preRenderAllTilemaps();
  void    preRenderTilemap(TileMap*, const QString& prefix);
  void    preRenderGround(TileMap*, const QString& prefix);
  void    preRenderLayers(const QList<TileLayer*>&, const QString& prefix);
  QString getPreRenderPath() const;
};

#endif // PRERENDERCOMPONENT_H
