#include "levelgrid.h"
#include "tilemap/tilezone.h"
#include "../objects/doorway.h"
#include <cmath>

bool LevelGrid::CaseContent::isBlocked() const
{
  if (!occupied)
  {
    for (const auto* zone : zones)
    {
      if (zone->getAccessBlocked())
        return true;
    }
    return false;
  }
  return true;
}

bool LevelGrid::CaseContent::isLinkedTo(QPoint position) const
{
  CaseContent* target;

  for (auto it = connections.begin() ; it != connections.end() ; ++it)
  {
    target = (*it)->getTargetFor(this);
    if (target->position == position && !target->isBlocked())
      return true;
  }
  return false;
}

LevelGrid::CaseConnection* LevelGrid::CaseContent::connectionWith(const CaseContent* other) const
{
  for (auto it = connections.begin() ; it != connections.end() ; ++it)
  {
    if ((*it)->getTargetFor(this) == other)
      return *it;
  }
  return nullptr;
}

int LevelGrid::CaseContent::apCostTo(const CaseContent* other) const
{
  auto* connection = connectionWith(other);

  return other ? connection->getCost() : 0;
}

void LevelGrid::CaseContent::connectWith(CaseContent* other)
{
  if (other)
  {
    CaseConnection* connection = new CaseConnection;

    connection->connect(this, other);
  }
}

void LevelGrid::CaseContent::disconnectFrom(CaseContent* other)
{
  for (auto* connection : connections)
  {
    if (connection->getTargetFor(this) == other)
    {
      connection->disconnect();
      break ;
    }
  }
}

void LevelGrid::CaseContent::clearConnections()
{
  while (connections.size())
    (*connections.begin())->disconnect();
}

void LevelGrid::CaseConnection::connect(CaseContent* a, CaseContent* b)
{
  pair = {a, b};
  a->disconnectFrom(b);
  b->disconnectFrom(a);
  a->connections.push_back(this);
  b->connections.push_back(this);
}

template<typename ARRAY, typename VALUE>
void removeFrom(ARRAY& array, VALUE value)
{
  auto it = std::find(array.begin(), array.end(), value);

  if (it != array.end())
      array.erase(it);
}

void LevelGrid::CaseConnection::disconnect()
{
  removeFrom(pair.first->connections, this);
  removeFrom(pair.second->connections, this);
  delete this;
}

int LevelGrid::CaseConnection::getCost() const
{
  if (doorway && !doorway->property("opened").toBool())
    return 3;
  if (pair.first->position.z != pair.second->position.z)
    return 3;
  return 1;
}

bool LevelGrid::CaseConnection::canGoThrough(CharacterMovement* character)
{
  if (doorway)
    return doorway->canGoThrough(reinterpret_cast<Character*>(character));
  return true;
}

bool LevelGrid::CaseConnection::goThrough(CharacterMovement* character)
{
  if (doorway)
    return doorway->onGoThrough(reinterpret_cast<Character*>(character));
  return true;
}

AstarPathfinding<LevelGrid::CaseContent>::Successors LevelGrid::CaseContent::GetSuccessors(const CaseContent* parent, Actor* actor) const
{
  AstarPathfinding<LevelGrid::CaseContent>::Successors results;

  for (auto* connection : connections)
  {
    CaseContent* node = connection->getTargetFor(this);

    if ((!parent || node->position != parent->position) && !node->isBlocked() && connection->canGoThrough(actor))
      results.push_back(node);
  }
  return results;
}

float LevelGrid::CaseContent::GoalDistanceEstimate(const CaseContent& other) const
{
  int distX = position.x - other.position.x;
  int distY = position.y - other.position.y;
  int distFloor = std::abs(position.z - other.position.x);

  return std::sqrt(static_cast<float>(distX * distX + distY * distY)) + static_cast<float>(distFloor * 10);
}
