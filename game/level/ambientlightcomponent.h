#ifndef  AMBIENTLIGHTCOMPONENT_H
# define AMBIENTLIGHTCOMPONENT_H

# include "camera.h"

class AmbientLightComponent : public CameraComponent
{
  Q_OBJECT
  typedef CameraComponent ParentType;

  Q_PROPERTY(QColor ambientColor    MEMBER ambientColor    NOTIFY ambientColorChanged)
  Q_PROPERTY(bool   useAmbientLight MEMBER useAmbientLight NOTIFY ambientColorChanged)
  Q_PROPERTY(bool   useDaylight     MEMBER useDaylight     NOTIFY ambientColorChanged)
public:
  explicit AmbientLightComponent(QObject *parent = nullptr);

  // TODO disable daylight on underground floors

  inline bool usesDaylight() const { return useAmbientLight && useDaylight; }

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

signals:
  void ambientColorChanged();
protected:
  void update(qint64);
private:
  void updateDaylight();

  QColor ambientColor;
  bool   useDaylight = false;
  bool   useAmbientLight = false;
};

#endif // AMBIENTLIGHTCOMPONENT_H
