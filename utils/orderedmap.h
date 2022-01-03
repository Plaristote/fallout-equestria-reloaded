#ifndef  ORDEREDMAP_H
# define ORDEREDMAP_H

# include <QMap>

template<typename KEY, typename VALUE>
class MapWithOrder : public QMap<KEY, VALUE>
{
public:
  MapWithOrder() {}

  MapWithOrder(const MapWithOrder& copy)
  {
    QMap<KEY, VALUE>::operator=(copy);
    _keys = copy._keys;
  }

  MapWithOrder& operator<<(const QPair<KEY, VALUE> entry)
  {
    _keys << entry.first;
    QMap<KEY, VALUE>::insert(entry.first, entry.second);
    return *this;
  }

  const QList<KEY>& keys() const { return _keys; }

private:
  QList<KEY> _keys;
};

#endif // ORDEREDMAP_H
