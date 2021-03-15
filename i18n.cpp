#include "i18n.h"
#include "globals.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QSettings>
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
  loadLocale();
  connect(this, &I18n::currentLocaleChanged, this, &I18n::loadLocale);
}

QString I18n::getSourceForLocale(const QString &locale)
{
  return ASSETS_PATH + "locales/" + locale + ".json";
}

void I18n::loadLocale()
{
  QFile file(getSourceForLocale(currentLocale));

  if (file.open(QIODevice::ReadOnly))
  {
    QSettings().setValue("locale", currentLocale);
    qDebug() << "i18n: Loading locale" << currentLocale;
    data = QJsonDocument::fromJson(file.readAll()).object();
    emit translationsChanged();
  }
  else
    qDebug() << "I18n: translation file not found for" << currentLocale;
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
