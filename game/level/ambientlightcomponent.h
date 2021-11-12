#ifndef  AMBIENTLIGHTCOMPONENT_H
# define AMBIENTLIGHTCOMPONENT_H

# include "prerender.h"

class AmbientLightComponent : public PreRenderComponent
{
  Q_OBJECT
  typedef PreRenderComponent ParentType;

  Q_PROPERTY(QColor ambientColor    MEMBER ambientColor    NOTIFY ambientColorChanged)
  Q_PROPERTY(bool   useAmbientLight MEMBER useAmbientLight NOTIFY ambientColorChanged)
  Q_PROPERTY(bool   useDaylight     MEMBER useDaylight     NOTIFY ambientColorChanged)
public:
  explicit AmbientLightComponent(QObject *parent = nullptr);

  void updateDaylight();
  inline bool usesDaylight() const { return useAmbientLight && useDaylight; }

signals:
  void ambientColorChanged();

private:
  QColor ambientColor;
  bool   useDaylight = false;
  bool   useAmbientLight = false;
};

#endif // AMBIENTLIGHTCOMPONENT_H
