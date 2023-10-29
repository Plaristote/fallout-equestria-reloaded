#ifndef I18N_H
#define I18N_H

#include <QObject>
#include <QJsonObject>
#include <QVariantMap>
#define DEFAULT_LOCALE "en"

class I18n : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString currentLocale MEMBER currentLocale NOTIFY currentLocaleChanged)
  Q_PROPERTY(QString consoleFont READ getConsoleFont NOTIFY currentLocaleChanged)
  Q_PROPERTY(QVariantMap consoleFontMetrics READ getConsoleFontMetrics NOTIFY currentLocaleChanged)
  Q_PROPERTY(QString titleFont READ getTitleFont NOTIFY currentLocaleChanged)
  Q_PROPERTY(QVariantMap titleFontMetrics READ getTitleFontMetrics NOTIFY currentLocaleChanged)

  static I18n* instance;
public:
  explicit I18n(QObject *parent = nullptr);

  static I18n* get() { return instance; }

  Q_INVOKABLE QStringList getAvailableLocales() const { return locales; }
  Q_INVOKABLE QString t(const QString& key) const;
  Q_INVOKABLE QString t(const QString& key, const QVariantMap& variables) const;
  const QString& getCurrentLocale() const { return currentLocale; }

  QString getConsoleFont() const;
  QVariantMap getConsoleFontMetrics() const;
  QString getTitleFont() const;
  QVariantMap getTitleFontMetrics() const;

  static QString getSourceForLocale(const QString& locale);
  static QString getSourceForLocale(const QString& translationFile, const QString& locale);

signals:
  void currentLocaleChanged();
  void translationsChanged();

public slots:
  void loadCurrentLocale();

private:
  QJsonValue  getTranslation(const QString& key) const;
  QJsonObject getTranslationGroupForKey(const QString& key) const;
  QJsonObject getTranslationGroup(const QStringList& path) const;
  QString     getFontPath(const QString& style, const QString& defaultPath) const;
  QVariantMap getFontMetrics(const QString& style, QVariantMap defaultMetrics) const;

  QString     currentLocale;
  QStringList locales;
  QJsonObject data;
};

#endif // I18N_H
