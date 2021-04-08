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

  static GameObjectTemplates* get() { return instance; }

  QStringList getNames() const { return data.keys(); }
  QJsonObject getObjectData(const QString& name) const { return data[name].toObject(); }

  Q_INVOKABLE void           save(const QString& name, DynamicObject*);
  Q_INVOKABLE DynamicObject* load(const QString& name);

  Q_INVOKABLE QString getObjectType(const QString& name) const     { return getObjectData(name)["_type"].toString(); }
  Q_INVOKABLE QString getSpriteGroup(const QString& name) const    { return getObjectData(name)["spriteName"].toString(); }
  Q_INVOKABLE QString getSpriteName(const QString& name) const     { return getObjectData(name)["animation"].toString(); }
  Q_INVOKABLE QString getScript(const QString& name) const         { return getObjectData(name)["script"].toString(); }
  Q_INVOKABLE QString getCharacterSheet(const QString& name) const { return getObjectData(name)["charsheet"].toString(); }

signals:
  void namesChanged();

private:
  QJsonObject data;
  static const QMap<QString, std::function<DynamicObject* (const QJsonObject&)>> typeMap;
  static GameObjectTemplates* instance;
};

#endif // GAMEOBJECTTEMPLATES_H
