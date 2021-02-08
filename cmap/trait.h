#ifndef  TRAIT_H
# define TRAIT_H

# include <QJSValue>

class StatModel;

class Trait
{
public:
  Trait();

  void toogle(StatModel*, bool);

  QString  name;
  QJSValue script;
};

#endif // TRAIT_H
