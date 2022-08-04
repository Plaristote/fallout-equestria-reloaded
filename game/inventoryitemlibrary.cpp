#include "globals.h"
#include "inventoryitemlibrary.h"
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

InventoryItemLibrary* InventoryItemLibrary::instance = nullptr;

InventoryItemLibrary::InventoryItemLibrary(QObject *parent) : QObject(parent)
{
  if (instance != nullptr)
    throw std::runtime_error("InventoryItemLibrary: only one instance allowed");
  instance = this;
}

InventoryItemLibrary::~InventoryItemLibrary()
{
  instance = nullptr;
}

void InventoryItemLibrary::initialize()
{
  QFile file(ASSETS_PATH + "objects.json");

  if (file.open(QIODevice::ReadOnly))
  {
    QJsonDocument document = QJsonDocument::fromJson(file.readAll());

    file.close();
    library = document.object();
  }
}

const QJsonValue InventoryItemLibrary::getObject(const QString& name) const
{
  return library[name];
}

QString InventoryItemLibrary::getIcon(const QString& name) const
{
  return library[name]["icon"].toString("any.png");
}

void InventoryItemLibrary::setObject(const QString& name, const QJsonObject& data)
{
  qDebug() << "InventoryItemLibrary::setObject" << name << QJsonDocument(data).toJson();
  library.remove(name);
  library.insert(name, data);
}

void InventoryItemLibrary::save()
{
  QFile file(ASSETS_PATH + "objects.json");

  if (file.open(QIODevice::WriteOnly))
  {
    qDebug() << "save: " << QJsonDocument(library).toJson();
    file.write(QJsonDocument(library).toJson());
    file.close();
  }
}
