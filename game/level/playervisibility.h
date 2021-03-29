#ifndef  PLAYERVISIBILITYCOMPONENT_H
# define PLAYERVISIBILITYCOMPONENT_H

# include "prerender.h"

class PlayerVisibilityComponent : public PreRenderComponent
{
  Q_OBJECT
  typedef PreRenderComponent ParentType;

  Q_PROPERTY(QQmlListProperty<Character> visibleCharacters READ getQmlVisibleCharacters NOTIFY visibleCharactersChanged)
public:
  PlayerVisibilityComponent(QObject* parent = nullptr);

  void load();

signals:
  void visibleCharactersChanged();

private slots:
  void refreshHiddenObjectsDetection();

private:
  virtual QQmlListProperty<Character> getQmlVisibleCharacters();

protected:
  QList<Character*> visibleCharacters;
};

#endif // PLAYERVISIBILITYCOMPONENT_H
