#ifndef  SAVEPREVIEW_H
# define SAVEPREVIEW_H

# include <QObject>

class SavePreview : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString      name           MEMBER name           NOTIFY nameChanged)
  Q_PROPERTY(QString      characterName  MEMBER characterName  NOTIFY dataChanged)
  Q_PROPERTY(unsigned int characterLevel MEMBER characterLevel NOTIFY dataChanged)
  Q_PROPERTY(QString      date           MEMBER date           NOTIFY dataChanged)
  Q_PROPERTY(QString      time           MEMBER time           NOTIFY dataChanged)
  Q_PROPERTY(QString      place          MEMBER place          NOTIFY dataChanged)
  Q_PROPERTY(QString      screenshot     READ screenshotPath   NOTIFY dataChanged)
public:
  explicit SavePreview(QObject *parent = nullptr);

  QString screenshotPath() const;

signals:
  void nameChanged();
  void dataChanged();

private slots:
  void update();
  
private:
  void loadFromJson(const QByteArray&);

  QString      name;
  QString      characterName;
  unsigned int characterLevel;
  QString      date, time;
  QString      place;
};

#endif // SAVEPREVIEW_H
