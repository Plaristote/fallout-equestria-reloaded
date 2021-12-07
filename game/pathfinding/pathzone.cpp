#include "pathzone.h"
#include <cmath>

float PathZone::GoalDistanceEstimate(const PathZone& other)
{
  Point position(*this);
  Point otherPosition(other);
  int distX = position.x - otherPosition.x;
  int distY = position.y - otherPosition.y;
  int distFloor = std::abs(position.z - otherPosition.x);

  return std::sqrt(static_cast<float>(distX * distX + distY * distY)) + static_cast<float>(distFloor * 10);
}

std::list<PathZone*> PathZone::GetSuccessors(const PathZone* parent, Actor* actor) const
{
  std::list<PathZone*> list;

  for (auto it = connections.keyBegin() ; it != connections.keyEnd() ; ++it)
  {
    if ((!parent || (*it)->id != parent->id) && isConnected(*it, actor))
      list.push_back(*it);
  }
  return list;
}

const PathZone::Connections* PathZone::getExitsTowards(const PathZone* target) const
{
  for (auto it = connections.begin() ; it != connections.end() ; ++it)
  {
    if (it.key()->id == target->id)
      return &(it.value());
  }
  return nullptr;
}

bool PathZone::isConnected(const PathZone* target, Actor* actor) const
{
  const Connections* candidates = getExitsTowards(target);

  if (candidates)
  {
    for (auto it = candidates->begin() ; it != candidates->end() ; ++it)
    {
      LevelGrid::CaseConnection* connection = it->first->connectionWith(it->second);

      if (connection && connection->canGoThrough(actor))
        return true;
    }
  }
  return false;
}

void PathZone::addExitTowards(PathZone* targetZone, Connection connection)
{
  auto it = connections.find(targetZone);

  if (it != connections.end())
    it->push_back(connection);
  else
    connections.insert(targetZone, {connection});
}

void PathZone::removeExitTowards(PathZone* targetZone, Connection connection)
{
  auto it = connections.find(targetZone);

  if (it != connections.end())
  {
    auto caseIt = std::find(it->begin(), it->end(), connection);

    if (caseIt != it->end())
    {
      if (it->size() == 1)
        connections.erase(it);
      else
        it->erase(caseIt);
    }
  }
}
