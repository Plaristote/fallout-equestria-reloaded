#ifndef  INVENTORYITEMLIBRARY_H
# define INVENTORYITEMLIBRARY_H

# include <QObject>
# include <QJsonObject>

class InventoryItemLibrary : public QObject
{
  Q_OBJECT

  static InventoryItemLibrary* instance;
public:
  explicit InventoryItemLibrary(QObject *parent = nullptr);
  ~InventoryItemLibrary();

  void initialize();

  static InventoryItemLibrary* get() { return instance; }

  Q_INVOKABLE const QJsonValue getObject(const QString&) const;
  Q_INVOKABLE QString getIcon(const QString&) const;
  Q_INVOKABLE void setObject(const QString&, const QJsonObject&);
  Q_INVOKABLE QStringList getObjectList() const { return library.keys(); }
  Q_INVOKABLE void save();

signals:

private:
  QJsonObject library;
};

#endif // INVENTORYITEMLIBRARY_H
