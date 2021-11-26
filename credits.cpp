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
  return persons[category][index];
}

CreditPerson::CreditPerson(QObject* parent) : QObject(parent)
{
}

void CreditPerson::initialize(const QJsonObject& data)
{
  auto currentDir = QDir::currentPath();

  name   = data["name"].toString();
  url    = data["url"].toString();
  for (const QString& filename : data["files"].toVariant().toStringList())
  {
    const auto path = currentDir + '/' + filename;

    if (QFile::exists(path))
      assets << ("file:/" + path);
    else
      assets << ("qrc:/" + filename);
  }
}
