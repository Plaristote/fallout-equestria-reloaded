#ifndef UNIQUECHARACTERSTORAGE_H
#define UNIQUECHARACTERSTORAGE_H

#include <QObject>

class UniqueCharacterStorage : public QObject
{
    Q_OBJECT
public:
    explicit UniqueCharacterStorage(QObject *parent = nullptr);

signals:

};

#endif // UNIQUECHARACTERSTORAGE_H
