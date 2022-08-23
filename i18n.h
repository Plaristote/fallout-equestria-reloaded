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

  static I18n* instance;
public:
  explicit I18n(QObject *parent = nullptr);

  static I18n* get() { return instance; }

  Q_INVOKABLE QStringList getAvailableLocales() const { return locales; }
  Q_INVOKABLE QString t(const QString& key) const;
  Q_INVOKABLE QString t(const QString& key, const QVariantMap& variables) const;
  const QString& getCurrentLocale() const { return currentLocale; }

  static QString getSourceForLocale(const QString& locale);

signals:
  void currentLocaleChanged();
  void translationsChanged();

public slots:
  void loadCurrentLocale();

private:
  QJsonObject getTranslationGroupForKey(const QString& key) const;
  QJsonObject getTranslationGroup(const QStringList& path) const;

  QString     currentLocale;
  QStringList locales;
  QJsonObject data;
};

#endif // I18N_H
