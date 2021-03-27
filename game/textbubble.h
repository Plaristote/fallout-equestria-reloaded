#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H

# include <QObject>
# include <QPoint>
# include <QString>

class DynamicObject;

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

#endif // TEXTBUBBLE_H
