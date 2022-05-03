#ifndef  TILEMAsK_H
# define TILEMAsK_H

# include <QObject>

class TileMask : public QObject
{
public:
  TileMask(QObject *parent = nullptr);
  Q_INVOKABLE virtual bool isInside(int x, int y, unsigned char z) const;
  Q_INVOKABLE virtual bool isInside(int x, int y) const = 0;
};

#endif
