#include "i18n.h"
#include "globals.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QSettings>
#include <QDir>
#include <QDebug>

I18n* I18n::instance = nullptr;

I18n::I18n(QObject *parent) : QObject(parent)
{
  QString defaultLocale = QSettings().value("locale", DEFAULT_LOCALE).toString();

  instance = this;
  QStringList files = QDir(ASSETS_PATH + "locales").entryList(QStringList() << "*.json", QDir::NoFilter, QDir::Name);
  for (QString& file : files)
    locales << file.replace(".json", "");
  if (locales.contains(defaultLocale))
    currentLocale = defaultLocale;
  else if (locales.size() > 0)
    currentLocale = locales.first();
  loadCurrentLocale();
  connect(this, &I18n::currentLocaleChanged, this, &I18n::loadCurrentLocale);
}

QString I18n::getSourceForLocale(const QString &locale)
{
  return ASSETS_PATH + "locales/" + locale + ".json";
}

static QStringList getSourcesForLocale(const QString& locale, const QString& basePath = ASSETS_PATH + "locales/")
{
  QDir directory(basePath);
  QString filter = "*." + locale + ".json";
  QFileInfoList list;
  QStringList files;
  QStringList bannedFolder{".", ".."};

  directory.setFilter(QDir::Dirs);
  list = directory.entryInfoList();
  for (int i = 0 ; i < list.size() ; ++i)
  {
    if (bannedFolder.indexOf(list.at(i).fileName()) >= 0) continue ;
    files << getSourcesForLocale(locale, basePath + list.at(i).fileName() + '/');
  }
  directory.setNameFilters(QStringList() << filter << (locale + ".json"));
  directory.setFilter(QDir::Files);
  list = directory.entryInfoList();
  for (int i = 0 ; i < list.size() ; ++i)
    files << (basePath + list.at(i).fileName());
  return files;
}

static void loadLocale(const QString& locale, QJsonObject& data)
{
  while (data.begin() != data.end()) data.erase(data.begin());
  for (const QString& filename : getSourcesForLocale(locale))
  {
    QFile file(filename);

    qDebug() << "i18n: Loading file" << filename;
    if (file.open(QIODevice::ReadOnly))
    {
      QJsonObject fileData = QJsonDocument::fromJson(file.readAll()).object();

      for (auto it = fileData.constBegin() ; it != fileData.constEnd() ; ++it)
        data.insert(it.key(), it.value());
    }
    else
      qDebug() << "i18n: failed to open file" << filename;
  }
}

void I18n::loadCurrentLocale()
{
  qDebug() << "i18n: Loading locale" << currentLocale;
  loadLocale(currentLocale, data);
  QSettings().setValue("locale", currentLocale);
  emit translationsChanged();
}

QString I18n::t(const QString &key) const
{
  QJsonObject group = getTranslationGroupForKey(key);
  QJsonValue value = group[key.split('.').last()];

  if (value.isUndefined() || value.isNull())
    return key;
  return value.toString();
}

QString I18n::t(const QString& key, const QVariantMap& variables) const
{
  QString str = t(key);

  for (const QString& varname : variables.keys())
  {
    QString hint("{{" + varname + "}}");

    str.replace(hint, variables[varname].toString());
  }
  return str;
}

QJsonObject I18n::getTranslationGroupForKey(const QString &key) const
{
  if (key.indexOf('.') > 0)
  {
    QStringList keys = key.split('.');

    keys.removeLast();
    return getTranslationGroup(keys);
  }
  return data;
}

QJsonObject I18n::getTranslationGroup(const QStringList &path) const
{
  QJsonObject group = data;

  for (const QString& key : path)
    group = group[key].toObject();
  return group;
}
