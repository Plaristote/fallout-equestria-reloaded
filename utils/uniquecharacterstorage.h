#ifndef UNIQUECHARACTERSTORAGE_H
#define UNIQUECHARACTERSTORAGE_H

#include <QObject>
#include <QQmlListProperty>
#include "game/character.h"

class UniqueCharacterStorage : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QQmlListProperty<Character> uniqueCharacterList READ getUniqueCharacterList)

public:
  explicit UniqueCharacterStorage(QObject *parent = nullptr);
  QQmlListProperty<Character> getUniqueCharacterList() { return QML_QLIST_CONSTRUCTOR(Character, uniqueCharacterList); }


signals:

private:
  QList<Character*> uniqueCharacterList;
};

#endif // UNIQUECHARACTERSTORAGE_H
