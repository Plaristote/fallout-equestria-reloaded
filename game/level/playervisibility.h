#ifndef  PLAYERVISIBILITYCOMPONENT_H
# define PLAYERVISIBILITYCOMPONENT_H

# include "prerender.h"

class PlayerVisibilityComponent : public PreRenderComponent
{
  Q_OBJECT
  typedef PreRenderComponent ParentType;

  Q_PROPERTY(QQmlListProperty<Character>     visibleCharacters READ getQmlVisibleCharacters NOTIFY visibleCharactersChanged)
  Q_PROPERTY(QQmlListProperty<DynamicObject> visibleObjects    READ getQmlVisibleObjects    NOTIFY visibleObjectsChanged)
public:
  PlayerVisibilityComponent(QObject* parent = nullptr);

  void load(const QJsonObject&);
  virtual void unregisterDynamicObject(DynamicObject*);

signals:
  void visibleCharactersChanged();
  void visibleObjectsChanged();

private slots:
  void refreshHiddenObjectsDetection();

private:
  virtual QQmlListProperty<Character>     getQmlVisibleCharacters();
  virtual QQmlListProperty<DynamicObject> getQmlVisibleObjects();

protected:
  QList<Character*>     visibleCharacters;
  QList<DynamicObject*> visibleObjects;
};

#endif // PLAYERVISIBILITYCOMPONENT_H
