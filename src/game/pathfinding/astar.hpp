/*
 * This AStar implementation has been inspired by Justin Heyes-Jones,
 * Most of the credit to him.
 */
#ifndef  ASTAR_HPP
# define ASTAR_HPP

//# include "globals.hpp"
# include <list>
# include <vector>
# include <map>
# include <algorithm>
# include <iostream>
# include <assert.h>

/*
 * A bit of documentation so you don't have to read this gorgeous piece of code:
 * UserState must implements at least
 * Actor type definition                                          : a type for the reference to the object moving through the nodes
 * bool  operator==()                                             : comparaison between two references to a UserState
 * float GetCost(UserState, Actor*)                               : the cost to go from a UserState to the one passed as parameter, for a given Actor
 * float GoalDistanceEstimate(UserState)                          : Heuristic between a UserState and another oneç
 * std::list<UserState*> GetSuccessors(UserState* parent, Actor*) : All the possible successor to the UserState, considering the parent and the Actor
 */
template <class UserState>
class AstarPathfinding
{
public:
  enum State
  {
    NotInitialized,
    Searching,
    Succeeded,
    Failed
  };

  // A lambda defining the limits of the search
  std::function<bool (const UserState&)> scope;

  // A node represents a possible state in the search
  // The user provided state type is included inside this type
  struct Node
  {
    Node() : parent(0), child(0), g(0.f), h(0.f), f(0.f) {}

    Node*     parent; // used during the search to record the parent of successor nodes
    Node*     child; // used after the search for the application to view the search in reverse
    float     g; // cost of this node + it's predecessors
    float     h; // heuristic estimate of distance to goal
    float     f; // sum of cumulative cost of predecessors and self and heuristic
    UserState userNode;
  };
  typedef std::vector<Node*>                    NodeList;
  typedef typename std::vector<Node*>::iterator NodeListIterator;
  typedef std::list<UserState>                  Solution;
  typedef std::list<UserState>                  CandidateList;
  typedef std::map<UserState, Solution>         SolutionList;

  struct FunctorCompareNode
  {
    bool operator() ( const Node *x, const Node *y ) const { return (x->f > y->f); }
  };

  AstarPathfinding(typename UserState::Actor* actor = nullptr) : _state(NotInitialized), _cancelRequest(false), _acceptAnyCandidate(false)
  {
    _actor          = actor;
    _allocateNodeIt = 0;
    _start          = 0;
    _goal           = 0;
  }

  ~AstarPathfinding()
  {
    if (_state == Succeeded)
      FreeSolutionNodes();
    FreeAllNodes();
  }

  void CancelSearch(void)
  {
    _cancelRequest = true;
  }

  void AcceptAnyCandidate(void)
  {
    _acceptAnyCandidate = true;
  }

  // Set Start and goal states
  void SetStartAndGoalStates(UserState& Start, const CandidateList& Candidates)
  {
    FreeSolutionNodes();

    _cancelRequest = false;

    _start = new Node;
    _goal  = new Node;

    assert(_start != NULL && _goal != NULL && Candidates.size() > 0);

    _start->userNode = Start;
    _goal->userNode  = *Candidates.begin();

    std::copy(++Candidates.begin(), Candidates.end(), std::back_inserter(_secondaryGoals));

    _state = Searching;

    _start->g = 0;
    _start->h = _start->userNode.GoalDistanceEstimate( _goal->userNode );
    _start->f = _start->g + _start->h;
    _start->parent = 0;

    // Push the start node on the Open list
    _openList.push_back(_start);

    // Sort back element into heap
    std::push_heap( _openList.begin(), _openList.end(), FunctorCompareNode() );

    // Initialise counter for search steps
    _nSteps = 0;
  }

  void  SearchSuccedded(Node* node)
  {
    _goal->parent = node->parent;

    // A special case is that the goal was passed in as the start state
    // so handle that here
    if (!(node->userNode == _start->userNode))
    {
      // set the child pointers in each node (except Goal which has no child)
      Node *nodeChild  = _goal;
      Node *nodeParent = _goal->parent;

      do
      {
        nodeParent->child = nodeChild;
        nodeChild         = nodeParent;
        nodeParent        = nodeParent->parent;
      }
      while(nodeChild != _start); // Start is always the first node by definition
      delete node;
    }
    // delete nodes that aren't needed for the solution
    FreeUnusedNodes();
    _state = Succeeded;
  }

  void StoreSecondarySolution(Node* node)
  {
    typename SolutionList::iterator currentSecondarySolution = _secondarySolutions.find(node->userNode);
    std::list<UserState> solution;
    Node*                current = node;

    while (current != _start)
    {
      solution.insert(solution.begin(), current->userNode);
      current = current->parent;
    }
    solution.insert(solution.begin(), _start->userNode);
    if (currentSecondarySolution == _secondarySolutions.end() || currentSecondarySolution->second.size() > solution.size())
      _secondarySolutions.emplace(node->userNode, solution);
  }

  Solution* GetBestSolution()
  {
    if (_state == Succeeded)
    {
      _secondarySolutions.emplace(_goal->userNode, GetSolution());
      auto it = _secondarySolutions.find(_goal->userNode);
      if (it != _secondarySolutions.end())
        return &(it->second);
    }
    for (const UserState& candidate : _secondaryGoals)
    {
      typename SolutionList::iterator solution = _secondarySolutions.find(candidate);

      if (solution != _secondarySolutions.end())
        return &(solution->second);
    }
    return nullptr;
  }

  State SearchStep()
  {
    if (_openList.empty() || _cancelRequest || _state == NotInitialized)
    {
      FreeAllNodes();
      _state = Failed;
    }
    else if (_state != Succeeded && _state != Failed)
    {
      _nSteps++;

      // Pop the best node (the one with the lowest f)
      Node* node = _openList.front(); // get pointer to the node
      std::pop_heap( _openList.begin(), _openList.end(), FunctorCompareNode() );
      _openList.pop_back();

      if (node->userNode == _goal->userNode)
        SearchSuccedded(node);
      else // not goal
      {
        std::list<UserState*> userSuccessors = node->userNode.GetSuccessors(node->parent ? &node->parent->userNode : 0, _actor);

        typename std::list<UserState*>::iterator successorIt  = userSuccessors.begin();
        typename std::list<UserState*>::iterator successorEnd = userSuccessors.end();

        if (std::find(_secondaryGoals.begin(), _secondaryGoals.end(), node->userNode) != _secondaryGoals.end())
        {
          StoreSecondarySolution(node);
          if (_acceptAnyCandidate)
          {
            CancelSearch();
            return _state;
          }
        }

        for (; successorIt != successorEnd; ++successorIt)
        {
          // Skip the successor unless it's part of the scope or goals
          if (scope && !scope(**successorIt) && !(_goal->userNode == **successorIt))
            continue ;

          Node* successor = new Node();

          successor->userNode = *(*successorIt);
          //      The g value for this successor ...
          float newg = node->g + node->userNode.GetCost(successor->userNode, _actor);

          // Now we need to find whether the node is on the open or closed lists
          // If it is but the node that is already on them is better (lower g)
          // then we can forget about this successor

          // First linear search of open list to find node

          NodeListIterator closedListSearch;
          NodeListIterator openListSearch;
          NodeListIterator listEnd;

          for(openListSearch = _openList.begin(), listEnd = _openList.end(); openListSearch != listEnd ; ++openListSearch)
          {
            if ((*openListSearch)->userNode == successor->userNode)
              break ;
          }
          if (openListSearch != listEnd)        // we found this state on open
          {
            if ((*openListSearch)->g <= newg) // the one on Open is cheaper than this one
            {
              delete successor;
              continue ;
            }
          }

          for(closedListSearch = _closedList.begin(), listEnd = _closedList.end() ; closedListSearch != listEnd ; ++closedListSearch)
          {
            if ((*closedListSearch)->userNode == successor->userNode)
              break;
          }
          if (closedListSearch != listEnd) // we found this state on closed
          {
            if( (*closedListSearch)->g <= newg ) // the one on Closed is cheaper than this one
            {
              delete successor;
              continue;
            }
          }

          // This node is the best node so far with this particular state
          // so lets keep it and set up its AStar specific data ...

          successor->parent = node;
          successor->g = newg;
          successor->h = successor->userNode.GoalDistanceEstimate( _goal->userNode );
          successor->f = successor->g + successor->h;

          // Remove successor from closed if it was on it

          if(closedListSearch != _closedList.end() )
          {
            // remove it from Closed
            delete (*closedListSearch);
            _closedList.erase(closedListSearch);
          }

          // Update old version of this node
          if(openListSearch != _openList.end())
          {
            delete (*openListSearch);
            _openList.erase(openListSearch);
            std::make_heap( _openList.begin(), _openList.end(), FunctorCompareNode() );
          }

          _openList.push_back(successor);                                        // heap now unsorted
          std::push_heap( _openList.begin(), _openList.end(), FunctorCompareNode() ); // sort back element into heap
        }
        // push n onto Closed, as we have expanded it now
        _closedList.push_back(node);
      } // end else (not goal so expand)
    }
    return _state; // Succeeded bool is false at this point.
  }

  // Free the solution nodes
  // This is done to clean up all used Node memory when you are done with the
  // search
  void FreeSolutionNodes()
  {
    Node *node = _start;

    if(node && node->child)
    {
      do
      {
        Node *del = node;
        node = node->child;
        delete del;

      } while(node != _goal);
      delete node;
    }
    else
    {
      if (_start) delete _start;
      if (_goal)  delete _goal;
    }
    _start = 0;
    _goal  = 0;
  }

  std::list<UserState> GetSolution(void)
  {
    std::list<UserState> solution;
    Node*                current = _start;

    while (current->child)
    {
      solution.push_back(current->userNode);
      current = current->child;
    }
    solution.push_back(current->userNode);
    return (solution);
  }

  int GetStepCount() { return _nSteps; }

private:
  // This is called when a search fails or is cancelled to free all used memory
  void FreeAllNodes()
  {
    FreeUnusedNodes(false);
    delete _goal;
    _goal = 0;
  }

  // This call is made by the search class when the search ends. A lot of nodes may be
  // created that are still present when the search ends. They will be deleted by this
  // routine once the search ends
  void FreeUnusedNodes(bool onlyUnused = true)
  {
    NodeListIterator it;
    NodeListIterator end;

    for (it = _openList.begin(), end = _openList.end() ; it != end ; ++it)
    {
      Node* node = *it;

      if (!onlyUnused || !node->child)
        delete node;
    }
    for (it = _closedList.begin(), end = _closedList.end() ; it != end ; ++it)
    {
      Node* node = *it;

      if (!onlyUnused || !node->child)
        delete node;
    }
    _openList.clear();
    _closedList.clear();
  }

  inline Node* PedirNode(void)
  {
    _allocateNodeIt++;
    return new Node();
  }
  
  inline void ReleaseNode(Node* node)
  {
    delete node;
    _allocateNodeIt--;
  }

  NodeList _openList;
  NodeList _closedList;
  State    _state;
  bool     _cancelRequest;
  bool     _acceptAnyCandidate;
  int      _nSteps;

  Node*         _start;
  Node*         _goal;
  CandidateList _secondaryGoals;
  SolutionList  _secondarySolutions;

  unsigned int _allocateNodeIt;
  typename UserState::Actor* _actor = nullptr;
};

#endif
