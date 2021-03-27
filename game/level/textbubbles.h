#ifndef  TEXTBUBBLESCOMPONENT_H
# define TEXTBUBBLESCOMPONENT_H

# include <QObject>
# include <QQmlListProperty>
# include "interaction.h"
# include "game/textbubble.h"

class TextBubblesComponent : public InteractionComponent
{
  Q_OBJECT
  typedef InteractionComponent ParentType;

  Q_PROPERTY(QQmlListProperty<TextBubble> textBubbles READ getQmlTextBubbles NOTIFY textBubblesChanged)
public:
  explicit TextBubblesComponent(QObject *parent = nullptr);

  QQmlListProperty<TextBubble> getQmlTextBubbles() { return QQmlListProperty<TextBubble>(this, &textBubbles); }

  void update(qint64 delta);

  Q_INVOKABLE void addTextBubble(DynamicObject* target, const QString& content, qint64 duration, const QString& color = "white");

  void unregisterDynamicObject(DynamicObject* object);

signals:
  void textBubblesChanged();

private:
  TextBubble* getTextBubbleFor(DynamicObject* target) const;

  QList<TextBubble*> textBubbles;
};

#endif // TEXTBUBBLESCOMPONENT_H
