#ifndef SOLVER_H
#define SOLVER_H

#include "Maze.h"
#include "Node.h"
#include <vector>
#include <set>
#include <string>
#include <queue>
#include <stack>
#include <iostream>

class Solver {
public:
    Solver(const Maze& maze);

    // Level 1: Blind Algorithms
    std::vector<char> solveBruteForce();
    std::vector<char> solveBFS();
    std::vector<char> solveDFS();

    // Level 2: Heuristic Algorithms
    std::vector<char> solveAStar();

private:
    // Static map data (constant throughout the search)
    std::vector<std::pair<int, int>> m_walls;
    std::vector<std::pair<int, int>> m_goals;
    int m_maxRow;
    int m_maxCol;

    // Initial state
    std::pair<int, int> m_startPlayer;
    std::set<std::pair<int, int>> m_startBoxes;

    // Helpers
    bool isWall(const std::pair<int, int>& pos) const;
    bool isGoal(const std::pair<int, int>& pos) const;
    bool isDeadlock(const std::set<std::pair<int, int>>& boxes) const;

    // Expansion
    std::vector<Node> expand(const Node& current) const;

    // Heuristic
    int calculateHeuristic(const Node& node) const;

    std::vector<std::vector<bool>> m_staticDeadlocks;

    bool bruteForceRecursive(Node& current, int depth, int maxDepth, std::vector<char>& solution);
};

#endif // SOLVER_H
