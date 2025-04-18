#ifndef REACHDOORACTION_H
#define REACHDOORACTION_H

# include "reach.h"
# include "game/objects/doorway.h"

class Doorway;

class ReachDoorAction : public ReachAction
{
public:
  ReachDoorAction(Character* character, Doorway* object, float range) : ReachAction(character, object, range), doorway(object)
  {
    candidates = getCandidates();
  }

  ReachDoorAction(Character* character, Doorway* object, float range, QJSValue callback) : ReachAction(character, object, range, callback), doorway(object)
  {
    candidates = getCandidates();
  }

  bool trigger() override;
  int getApCost() const override;

private:
  QVector<Point> getCandidates() const;
  QVector<Point> getCandidatesFromZone(TileZone*) const;
  bool alreadyReached() const override;
  QList<Point> getPath() const;

  Doorway* doorway;
  QVector<Point> candidates;
};

#endif // REACHDOORACTION_H
