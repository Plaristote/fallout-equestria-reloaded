#include <QMovie>
#include <QPainter>
#include <QPainter>
#include <QJsonObject>
#include <QJsonDocument>
#include <QGuiApplication>
#include <QFile>

static QSize getAnimationSize(const QMovie& animation, QRect rect)
{
  return QSize(rect.width() * animation.frameCount(), rect.height());
}

static QRect getAnimationLimits(QMovie& animation)
{
  QPoint min, max, margin;
  auto frameSize = animation.currentImage().size();

  min = {frameSize.width(), frameSize.height()};
  for (int i = 0 ; i < animation.frameCount() ; ++i)
  {
    animation.jumpToFrame(i);
    auto image = animation.currentImage();
    for (int x = 0 ; x < image.width() ; ++x)
    {
      for (int y = 0 ; y < image.height() ; ++y)
      {
        if (image.pixelColor(x, y) != Qt::transparent)
        {
          int neighborCount = 0;
          for (int xx = x - 1 ; xx < x + 1 ; ++xx)
          {
            for (int yy = y - 1 ; yy < y + 1 ; ++yy)
            {
              if (image.rect().contains(xx, yy))
              {
                if (image.pixelColor(xx, yy) != Qt::transparent)
                  neighborCount++;
              }
            }
          }
          if (neighborCount < 3)
            continue ;
          if (x < min.x())
            min.setX(x);
          if (x > max.x())
            max.setX(x);
          if (y < min.y())
            min.setY(y);
          if (y > min.y())
            max.setY(y);
        }
      }
    }
  }
  margin = {frameSize.width() - max.x(), frameSize.height() - max.y()};
  min = {
    std::max<int>(0, min.x() - margin.x()),
    std::max<int>(0, min.y() - margin.y())
  };
  animation.jumpToFrame(0);
  return QRect(
     min,
     QSize(frameSize.width() - min.x(), frameSize.height() - min.y())
  );
}

static void exportJsonData(const QMovie& animation, QRect rect, QJsonObject& animationData)
{
  animationData["frameCount"] = animation.frameCount();
  animationData["height"] = rect.height();
  animationData["width"] = rect.width();
}

static QImage convertGif(const QString& source, QPoint offset, QJsonObject& animationData)
{
  qDebug() << "Loading GIF" << source;
  QMovie   animation(source); animation.jumpToFrame(0);
  QRect    rect = getAnimationLimits(animation);
  QImage   image(getAnimationSize(animation, rect), QImage::Format_ARGB32);
  QPainter painter;

  qDebug() << "Animation size:" << getAnimationSize(animation, rect) << "rect" << rect;
  exportJsonData(animation, rect, animationData);
  image.fill(Qt::transparent);
  painter.begin(&image);
  for (int i = 0 ; i < animation.frameCount() ; ++i)
  {
    animation.jumpToFrame(i);
    painter.drawImage(
      QRect(QPoint(rect.width() * i, 0), rect.size()),
      animation.currentImage(),
      QRect(offset + rect.topLeft(), rect.size())
    );
  }
  painter.end();
  return image;
}

static QImage renderImageList(const QList<QImage>& images, QSize size)
{
  QImage image(size, QImage::Format_ARGB32);
  QPainter painter;

  image.fill(Qt::transparent);
  painter.begin(&image);
  int y = 0;
  for (const QImage& animationImage : images)
  {
    painter.drawImage(0, y, animationImage);
    y += animationImage.height();
  }
  painter.end();
  return image;
}

static QJsonObject makeDeadAnimation(const QJsonObject& animationData)
{
  QJsonObject deadAnimation;

  deadAnimation["offsetX"] = (animationData["frameCount"].toInt() - 1) * animationData["width"].toInt();
  deadAnimation["offsetY"] = animationData["offsetY"].toInt();
  deadAnimation["width"]   = animationData["width"].toInt();
  deadAnimation["height"]  = animationData["height"].toInt();
  return deadAnimation;
}

static QImage addAnimationWithDirections(const QString& name, const QString& alias, QPoint offset, int sheetOffset, QJsonObject& spriteData)
{
  QList<QImage> images;
  QSize         size;
  static QHash<QString, QString> directionDictionary{
    {"N", "up"}, {"W", "left"}, {"S", "down"}, {"E", "right"}
  };

  for (const QString& direction : directionDictionary.keys())
  {
    qDebug() << "-- importing direction" << directionDictionary[direction];
    QJsonObject animationData;
    QString     source = name + '_' + direction + ".gif";
    QImage      image  = convertGif(source, offset, animationData);

    if (image.height() == 0 || image.width() == 0)
      continue ;
    animationData["frameInterval"] = 70;
    animationData["offsetY"]       = size.height() + sheetOffset;
    if (alias == "walking" || alias == "running")
      animationData["repeat"] = true;
    if (alias == "fall-down")
      spriteData["dead-" + directionDictionary[direction]] = makeDeadAnimation(animationData);
    spriteData[alias + '-' + directionDictionary[direction]] = animationData;
    size.rheight() += image.height();
    if (size.width() < image.width())
      size.setWidth(image.width());
    images.push_back(image);
  }
  return renderImageList(images, size);
}

QImage makeSpritesheet(const QString& path, QPoint offset, QJsonObject& spriteData)
{
  QList<QImage> images;
  QSize         size;
  static QHash<QString, QString> animationDictionary{
    {"Default",               "idle"},
    {"default",               "idle"},
    {"StandBreathe",          "idle"},
    {"Death",                 "fall-down"},
    {"StandGetupback",        "get-up"},
    {"StandRecoil",           "damaged"},
    {"StandWalk",             "walking"},
    {"StandRun",              "running"},
    {"StandDodge",            "dodge"},
    {"StandMagichigh",        "use"},
    {"StandAttackUnarmedOne", "melee"},
    {"StandAttackUnarmedTwo", "melee-alt"},

    {"StandAttackRifleSingle",  "fire-rifle"},
    {"StandAttackRifleBurst",   "fire-rifle-burst"},
    {"StandAttackRiflePistol",  "fire-pistol"},
    {"StandAttackRocketSingle", "fire-rocket"},
    {"StandAttackMinigunBurst", "fire-minigun"},
    {"StandAttackThrow",        "throw"},
    {"StandAttackSpearThurst",  "melee-spear"},
    {"StandAttackSpearThrow",   "throw-spear"}
  };

  qDebug() << "sprite source:" << path;
  for (const QString& key : animationDictionary.keys())
  {
    qDebug() << "- importing animation" << animationDictionary[key];
    QImage image = addAnimationWithDirections(path + '/' + key, animationDictionary[key], offset, size.height(), spriteData);

    images.push_back(image);
    size.rheight() += image.height();
    if (size.width() < image.width())
      size.setWidth(image.width());
  }
  return renderImageList(images, size);
}

int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);
  QString     source(argv[1]);
  QString     output(argv[2]);
  QString     outputPath = "characters/" + output + ".png";
  QPoint      offset;
  QJsonObject data;
  QImage      image = makeSpritesheet(source, offset, data);

  if (image.save("assets/sprites/" + outputPath))
  {
    QFile spriteJson("assets/sprites.json");

    if (spriteJson.open(QIODevice::ReadWrite))
    {
      auto spriteDocument = QJsonDocument::fromJson(spriteJson.readAll()).object();
      QByteArray json;

      data["defaultSource"] = outputPath;
      spriteDocument.insert(output, data);
      json = QJsonDocument(spriteDocument).toJson(QJsonDocument::Indented);
      spriteJson.seek(0);
      spriteJson.write(QJsonDocument(spriteDocument).toJson(QJsonDocument::Indented));
      spriteJson.resize(json.length());
      qDebug() << "saved sprites.json";
    }
    else
      qDebug() << "could not save sprites.json";
    qDebug() << "saved texture at:" << outputPath;
  }
  else
    qDebug() << "could not save texture";
  return 0;
}
