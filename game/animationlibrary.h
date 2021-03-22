#ifndef  ANIMATIONLIBRARY_H
# define ANIMATIONLIBRARY_H

# include <QObject>
# include <QJsonObject>
# include <QRect>
# include <QImage>

struct SpriteAnimation
{
  QString name;
  QString source;
  QPoint  firstFramePosition;
  QRect   clippedRect;
  int     frameCount;
  int     frameInterval;
  bool    repeat;
  int     currentFrame = 0;
};

class QmlSpriteAnimation : public QObject, public SpriteAnimation
{
  Q_OBJECT

  Q_PROPERTY(QString name   MEMBER name NOTIFY nameChanged)
  Q_PROPERTY(QString source MEMBER source NOTIFY sourceChanged)
  Q_PROPERTY(QString relativeSource READ getRelativeSource NOTIFY sourceChanged)
  Q_PROPERTY(QPoint offset MEMBER firstFramePosition NOTIFY firstFramePositionChanged)
  Q_PROPERTY(QRect clippedRect MEMBER clippedRect NOTIFY clippedRectChanged)
  Q_PROPERTY(int frameCount MEMBER frameCount NOTIFY frameCountChanged)
  Q_PROPERTY(int frameInterval MEMBER frameInterval NOTIFY frameIntervalChanged)
  Q_PROPERTY(bool repeat MEMBER repeat NOTIFY repeatChanged)
public:
  QmlSpriteAnimation(QObject* parent = nullptr) : QObject(parent)
  {
  }

  Q_INVOKABLE void initialize(const QString& group, const QString& name);
  QString getRelativeSource() const;

signals:
  void frameIntervalChanged();
  void frameCountChanged();
  void firstFramePositionChanged();
  void sourceChanged();
  void nameChanged();
  void repeatChanged();
  void clippedRectChanged();

private:
};

class AnimationLibrary : public QObject
{
  Q_OBJECT
  static AnimationLibrary* self;
public:
  explicit AnimationLibrary(QObject *parent = nullptr);
  virtual ~AnimationLibrary() override;

  static AnimationLibrary* get() { return self; }
  void initialize();
  SpriteAnimation getAnimation(const QString& group, const QString& animation) const;
  const QImage& getImage(const QString& group, const QString& animation) const;
  const QImage& getImage(const QString& source) const;

  Q_INVOKABLE QStringList getSources() const { return textures; }
  Q_INVOKABLE QStringList getGroups() const;
  Q_INVOKABLE QStringList getAnimationList(const QString& group) const;

  // Editor
  Q_INVOKABLE void setAnimation(const QString& group, const QString& name, QmlSpriteAnimation*);
  Q_INVOKABLE void save();
  // END Editor

signals:
  void initialized();

private:
  QStringList           textures;
  QMap<QString, QImage> images;
  QJsonObject           data;
};

#endif // ANIMATIONLIBRARY_H
