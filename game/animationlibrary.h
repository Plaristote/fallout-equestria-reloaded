#ifndef  ANIMATIONLIBRARY_H
# define ANIMATIONLIBRARY_H

# include <QObject>
# include <QJsonDocument>
# include <QRect>

struct SpriteAnimation
{
  QString source;
  QPoint  firstFramePosition;
  QRect   clippedRect;
  int     frameCount;
  int     frameInterval;
  bool    repeat;
  int     currentFrame = 0;
};

class AnimationLibrary : public QObject
{
  Q_OBJECT
  static AnimationLibrary* self;
public:
  explicit AnimationLibrary(QObject *parent = nullptr);
  virtual ~AnimationLibrary();

  static AnimationLibrary* get() { return self; }
  void initialize();
  SpriteAnimation getAnimation(const QString& group, const QString& animation) const;

  Q_INVOKABLE QStringList getSources() const { return textures; }

signals:
  void initialized();

private:
  QStringList   textures;
  QJsonDocument data;
};

#endif // ANIMATIONLIBRARY_H
