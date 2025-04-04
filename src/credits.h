#ifndef  CREDITS_H
# define CREDITS_H

# include <QObject>
# include <QStringList>
# include <QQmlListProperty>

class QJsonObject;

class CreditPerson : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString     name   MEMBER name   CONSTANT)
  Q_PROPERTY(QString     url    MEMBER url    CONSTANT)
  Q_PROPERTY(QString     avatar MEMBER avatar CONSTANT)
  Q_PROPERTY(QStringList assets MEMBER assets CONSTANT)
public:
  CreditPerson(QObject* parent = nullptr);

  void initialize(const QJsonObject&);
  bool hasAssets() const { return assets.size() > 0; }

private:
  QString     name, url, avatar;
  QStringList assets;
};

class Credits : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QStringList categories MEMBER categories CONSTANT)
public:
  explicit Credits(QObject *parent = nullptr);

  Q_INVOKABLE unsigned short categoryCount(const QString&) const;
  Q_INVOKABLE CreditPerson*  person(const QString&, unsigned short index) const;
  Q_INVOKABLE bool           categoryHasArtwork(const QString&) const;

private:
  QStringList                           categories;
  QMap<QString, QList<CreditPerson*>> persons;
};

#endif // CREDITS_H
