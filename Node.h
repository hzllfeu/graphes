#ifndef NODE_H
#define NODE_H

#include <vector>
#include <set>
#include <string>
#include <utility>

struct Node {
    std::pair<int, int> playerPos;
    std::set<std::pair<int, int>> boxes; // Using set for automatic sorting and easy comparison
    std::string path; // Stores the sequence of moves (0: TOP, 1: BOTTOM, 2: LEFT, 3: RIGHT)

    // For A*
    int cost = 0; // g(n)
    int heuristic = 0; // h(n)

    int f() const { return cost + heuristic; }

    // Operator for std::set (visited states)
    bool operator<(const Node& other) const {
        if (playerPos != other.playerPos)
            return playerPos < other.playerPos;
        return boxes < other.boxes;
    }

    bool operator==(const Node& other) const {
        return playerPos == other.playerPos && boxes == other.boxes;
    }
};

struct NodeComparator {
    bool operator()(const Node& a, const Node& b) const {
        return a.f() > b.f(); // Min-priority queue for A*
    }
};

#endif // NODE_H
