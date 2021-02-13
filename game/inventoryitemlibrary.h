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

  const QJsonValue getObject(const QString&)const;

signals:

private:
  QJsonObject library;
};

#endif // INVENTORYITEMLIBRARY_H
