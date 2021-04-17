#ifndef STORABLEOBJECT_H
# define STORABLEOBJECT_H

# include <QObject>
# include <QJsonObject>
# include <QVariant>

class StorableObject : public QObject
{
  Q_OBJECT
public:
  explicit StorableObject(QObject *parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  Q_INVOKABLE inline bool     hasVariable(const QString& name) const { return dataStore.contains(name); }
  Q_INVOKABLE inline QVariant getVariable(const QString& name) const { return dataStore[name].toVariant(); }
  Q_INVOKABLE inline void     setVariable(const QString& name, const QVariant& value) { dataStore.insert(name, QJsonValue::fromVariant(value)); }
  Q_INVOKABLE inline void     unsetVariable(const QString& name) { dataStore.remove(name); }

protected:
  QJsonObject dataStore;
};

#endif // STORABLEOBJECT_H
