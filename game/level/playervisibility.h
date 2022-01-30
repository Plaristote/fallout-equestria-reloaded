#ifndef  PLAYERVISIBILITYCOMPONENT_H
# define PLAYERVISIBILITYCOMPONENT_H

# include "ambientlightcomponent.h"

class PlayerVisibilityComponent : public AmbientLightComponent
{
  Q_OBJECT
  typedef AmbientLightComponent ParentType;

  Q_PROPERTY(QQmlListProperty<Character>     visibleCharacters READ getQmlVisibleCharacters NOTIFY visibleCharactersChanged)
  Q_PROPERTY(QQmlListProperty<DynamicObject> visibleObjects    READ getQmlVisibleObjects    NOTIFY visibleObjectsChanged)
public:
  PlayerVisibilityComponent(QObject* parent = nullptr);

  void load(const QJsonObject&);
  virtual void registerDynamicObject(DynamicObject*);
  virtual void unregisterDynamicObject(DynamicObject*);

signals:
  void visibleCharactersChanged();
  void visibleObjectsChanged();

private slots:
  void refreshCharacterDetection();
  void refreshHiddenObjectsDetection();

private:
  virtual QQmlListProperty<Character>     getQmlVisibleCharacters();
  virtual QQmlListProperty<DynamicObject> getQmlVisibleObjects();

protected:
  QList<Character*>     visibleCharacters;
  QList<DynamicObject*> visibleObjects;
};

#endif // PLAYERVISIBILITYCOMPONENT_H
