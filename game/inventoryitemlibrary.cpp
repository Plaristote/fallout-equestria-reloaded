#include "globals.h"
#include "inventoryitemlibrary.h"
#include <QJsonDocument>
#include <QFile>

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

    library = document.object();
  }
}

const QJsonValue InventoryItemLibrary::getObject(const QString& name) const
{
  return library[name];
}
