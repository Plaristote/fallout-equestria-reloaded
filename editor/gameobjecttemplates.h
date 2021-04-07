#ifndef  GAMEOBJECTTEMPLATES_H
# define GAMEOBJECTTEMPLATES_H

# include <QObject>
# include <QJsonObject>

class DynamicObject;

class GameObjectTemplates : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QStringList names READ getNames NOTIFY namesChanged)
public:
  explicit GameObjectTemplates(QObject *parent = nullptr);

  void initialize();
  void save();

  QStringList getNames() const { return data.keys(); }
  QJsonObject getObjectData(const QString& name) const { return data[name].toObject(); }

  Q_INVOKABLE void           save(const QString& name, DynamicObject*);
  Q_INVOKABLE DynamicObject* load(const QString& name);

signals:
  void namesChanged();

private:
  QJsonObject data;
  static const QMap<QString, std::function<DynamicObject* (const QJsonObject&)>> typeMap;
};

#endif // GAMEOBJECTTEMPLATES_H
