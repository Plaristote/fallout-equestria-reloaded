#ifndef GAMEPACKAGES_H
#define GAMEPACKAGES_H

#include <QObject>
#include <QJsonObject>
#include <QQmlListProperty>
#include "globals.h"

class GamePackage : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name CONSTANT)
  Q_PROPERTY(QString description MEMBER description CONSTANT)
  Q_PROPERTY(QString logoPath MEMBER logoPath CONSTANT)
public:
  explicit GamePackage(const QString& name, QObject* parent = nullptr);
  explicit GamePackage(QObject* parent = nullptr);

  void initialize(const QJsonObject&);

  const QString& getName() const { return name; }
  const QString& getLogoPath() const { return logoPath; }
  const QString& getGamePath() const { return gamePath; }
  const QString& getOrganizationName() const { return organizationName; }
  const QString& getOrganizationDomain() const { return organizationDomain; }
  const QString& getApplicationName() const { return applicationName; }

private:
  QString name, description;
  QString logoPath;
  QString gamePath;
  QString organizationName;
  QString organizationDomain;
  QString applicationName;
};

class GamePackages : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QQmlListProperty<GamePackage> packages READ getQmlPackages CONSTANT)
public:
  explicit GamePackages(QObject *parent = nullptr);

  const QList<GamePackage*>& getPackages() const { return packages; }
  QQmlListProperty<GamePackage> getQmlPackages() { return QML_QLIST_CONSTRUCTOR(GamePackage, packages); }

signals:
  void pickedPackage(const GamePackage* package);

private:
  QList<GamePackage*> packages;
};

#endif // GAMEPACKAGES_H
