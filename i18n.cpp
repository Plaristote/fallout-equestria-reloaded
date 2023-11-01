#include "i18n.h"
#include "globals.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QSettings>
#include <QDir>
#include <QLocale>
#include <QDebug>

I18n* I18n::instance = nullptr;
void merge(QJsonObject&, const QJsonObject&);

I18n::I18n(QObject *parent) : QObject(parent)
{
  QString defaultLocale;

  instance = this;
  QStringList files = QDir(ASSETS_PATH + "locales").entryList(QStringList() << "*.json", QDir::NoFilter, QDir::Name);
  for (QString& file : files)
  {
    file.replace(".json", "");
    if (file.indexOf('.') == -1)
      locales << file;
  }
  defaultLocale = QSettings().value("locale", getSystemLocale()).toString();
  if (locales.contains(defaultLocale))
    currentLocale = defaultLocale;
  else if (locales.size() > 0)
    currentLocale = locales.first();
  loadCurrentLocale();
  connect(this, &I18n::currentLocaleChanged, this, &I18n::loadCurrentLocale);
}

QString I18n::getSystemLocale() const
{
  QStringList uiLanguages = QLocale().uiLanguages();

  qDebug() << "getSystemLocale" << uiLanguages;
  for (const QString& locale : uiLanguages)
  {
    if (locales.contains(locale))
      return locale;
  }
  return DEFAULT_LOCALE;
}

QString I18n::getSourceForLocale(const QString &locale)
{
  return ASSETS_PATH + "locales/" + locale + ".json";
}

QString I18n::getSourceForLocale(const QString& translationFile, const QString &locale)
{
  const auto base_path = ASSETS_PATH + "locales/" + translationFile;

  if (*translationFile.rbegin() == '/')
    return base_path + locale + ".json";
  return base_path + '.' + locale + ".json";
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

      merge(data, fileData);
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

QString I18n::getFontPath(const QString& style, const QString& defaultPath) const
{
  QJsonValue fontFile = getTranslation("fonts." + style + ".file");

  if (fontFile.isUndefined() || fontFile.isNull())
    return defaultPath;
  return ASSETS_PATH + "fonts/" + fontFile.toString();
}

QVariantMap I18n::getFontMetrics(const QString& style, QVariantMap metrics) const
{
  QJsonObject fontData = getTranslationGroupForKey("fonts." + style);

  if (fontData.find("point-size") != fontData.end())
    metrics.insert("point", fontData.find("point-size").value().toInt());
  if (fontData.find("tiny-size") != fontData.end())
    metrics.insert("tiny", fontData.find("tiny-size").value().toInt());
  if (fontData.find("big-size") != fontData.end())
    metrics.insert("big", fontData.find("big-size").value().toInt());
  return metrics;
}

QString I18n::getConsoleFont() const
{
  return getFontPath("console", "qrc:/assets/fonts/JH_FALLOUT.TTF");
}

QVariantMap I18n::getConsoleFontMetrics() const
{
  return getFontMetrics("console", {{"point", 9}, {"tiny", 8}, {"big", 12}});
}

QString I18n::getTitleFont() const
{
  return getFontPath("title", "qrc:/assets/fonts/fallout.ttf");
}

QVariantMap I18n::getTitleFontMetrics() const
{
  return getFontMetrics("title", {{"point", 18}, {"tiny", 24}, {"big", 32}});
}

QJsonValue I18n::getTranslation(const QString& key) const
{
  QJsonObject group = getTranslationGroupForKey(key);

  return group[key.split('.').last()];
}

QString I18n::t(const QString &key) const
{
  QJsonValue value = getTranslation(key);

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
