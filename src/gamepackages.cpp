#include "gamepackages.h"
#include "i18n.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

GamePackages::GamePackages(QObject *parent)
  : QObject{parent}
{
  QDir dir(".");

  for (QFileInfo fileInfo : dir.entryInfoList(QStringList() << "*.game.json", QDir::Files))
  {
    QFile file(fileInfo.filePath());

    if (file.open(QIODevice::ReadOnly))
    {
      GamePackage* package = new GamePackage(fileInfo.fileName(), this);
      QByteArray json = file.readAll();
      QJsonDocument document = QJsonDocument::fromJson(json);

      package->initialize(document.object());
      packages << package;
    }
  }
}

GamePackage::GamePackage(const QString& name, QObject *parent)
    : QObject{parent}, applicationName(name)
{
}

GamePackage::GamePackage(QObject *parent)
  : QObject{parent}
{
}

void GamePackage::initialize(const QJsonObject& object)
{
  I18n i18n;
  QString currentLocale = i18n.getCurrentLocale();
  QJsonValue nameValue = object["name"];
  QJsonObject nameObject = nameValue.toObject();
  QJsonValue descriptionValue = object["description"];
  QJsonObject descriptionObject = descriptionValue.toObject();

  organizationName = object["organizationName"].toString();
  organizationDomain = object["organizationDomain"].toString();
  gamePath = object["path"].toString();
  logoPath = object["logo"].toString();
  if (nameValue.isString())
    name = nameValue.toString();
  else if (nameObject[currentLocale].isString())
    name = nameObject[currentLocale].toString();
  else
    name = nameObject["default"].toString();
  if (descriptionValue.isString())
    description = descriptionValue.toString();
  else if (descriptionObject[currentLocale].isString())
    description = descriptionObject[currentLocale].toString();
  else
    description = descriptionObject["default"].toString();
}
