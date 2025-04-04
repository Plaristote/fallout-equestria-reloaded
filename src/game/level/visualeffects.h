#ifndef  VISUAL_EFFECTS_COMPONENT_H
# define VISUAL_EFFECTS_COMPONENT_H

# include "textbubbles.h"
# include "../animationSequence/animationsequence.h"

class VisualEffectsComponent : public TextBubblesComponent
{
  Q_OBJECT
  typedef TextBubblesComponent ParentType;

  Q_PROPERTY(QQmlListProperty<Sprite> visualEffects READ getQmlVisualEffects NOTIFY visualEffectsChanged)
public:
  explicit VisualEffectsComponent(QObject* parent = nullptr);
  virtual ~VisualEffectsComponent() override;

  virtual void registerVisualEffect(Sprite*);
  virtual void unregisterVisualEffect(Sprite*);
  void updateVisualEffects(qint64);

  Q_INVOKABLE void addAnimationSequence(QJSValue descriptor);
  void addAnimationSequence(AnimationSequence*, QJSValue callback = QJSValue());

  QQmlListProperty<Sprite> getQmlVisualEffects() { return QML_QLIST_CONSTRUCTOR(Sprite, visualEffects); }

signals:
  void visualEffectsChanged();

private:
  void updateSprites(qint64);
  void updateRunningAnimations();

  QList<Sprite*> visualEffects;
  QVector<QPair<AnimationSequence*, QJSValue>> runningAnimations;
};

#endif
