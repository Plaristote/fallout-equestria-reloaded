#ifndef  TEXTBUBBLESCOMPONENT_H
# define TEXTBUBBLESCOMPONENT_H

# include <QObject>
# include <QQmlListProperty>
# include "interaction.h"

class TextBubble : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString content  MEMBER content  NOTIFY contentChanged)
  Q_PROPERTY(QString color    MEMBER color    NOTIFY colorChanged)
  Q_PROPERTY(QPoint  position MEMBER position NOTIFY positionChanged)
  Q_PROPERTY(float   opacity  READ getOpacity NOTIFY opacityChanged)
public:
  TextBubble(QObject* parent = nullptr) : QObject(parent), target(nullptr) {}
  TextBubble(QObject* parent, DynamicObject* target);

  void initialize(const QString& content, qint64 duration, const QString& color);
  bool update(qint64 delta);

  float getOpacity() const;
  DynamicObject* getTarget() const { return target; }

signals:
  void contentChanged();
  void colorChanged();
  void positionChanged();
  void opacityChanged();

private:
  QString        content, color;
  QPoint         position;
  DynamicObject* target;
  qint64         timeLeft;
};

class TextBubblesComponent : public InteractionComponent
{
  Q_OBJECT

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
  QList<TextBubble*> textBubbles;
};

#endif // TEXTBUBBLESCOMPONENT_H
