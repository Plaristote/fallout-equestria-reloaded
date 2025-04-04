#include "credits.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

Credits::Credits(QObject *parent) : QObject(parent)
{
  QFile source("assets/credits/credits.json");

  if (source.open(QIODevice::ReadOnly))
  {
    const auto list = QJsonDocument::fromJson(source.readAll()).array();

    for (const auto& personValue : list)
    {
      CreditPerson* person = new CreditPerson(this);
      QJsonObject   data = personValue.toObject();
      QString       category = data["category"].toString();

      person->initialize(data);
      if (!categories.contains(category))
      {
        categories.push_back(category);
        persons.insert(category, {person});
      }
      else
        persons[category].push_back(person);
    }
  }
}

unsigned short Credits::categoryCount(const QString& category) const
{
  return persons[category].size();
}

CreditPerson* Credits::person(const QString& category, unsigned short index) const
{
  if (persons[category].count() > index)
    return persons[category][index];
  return nullptr;
}

bool Credits::categoryHasArtwork(const QString& category) const
{
  for (auto* person : persons[category])
  {
    if (person->hasAssets())
      return true;
  }
  return false;
}

CreditPerson::CreditPerson(QObject* parent) : QObject(parent)
{
}

static QString getCreditResourcePath(const QString& filename)
{
  const auto currentDir = QDir::currentPath();
  const auto path = currentDir + '/' + filename;

  if (QFile::exists(path))
    return ("file:/" + path);
  return ("qrc:/" + filename);
}

void CreditPerson::initialize(const QJsonObject& data)
{
  auto currentDir = QDir::currentPath();

  name   = data["name"].toString();
  url    = data["url"].toString();
  if (data["avatar"].isUndefined())
    avatar = getCreditResourcePath("assets/credits/no-avatar.png");
  else
    avatar = getCreditResourcePath("assets/credits/" + data["avatar"].toString());
  for (const QString& filename : data["files"].toVariant().toStringList())
    assets << getCreditResourcePath(filename);
}
