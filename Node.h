#ifndef NODE_H
#define NODE_H

#include <vector>
#include <set>
#include <string>
#include <utility>
#include <functional> // For std::hash

// Custom hash for std::pair
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        // Simple bit mixing
        return h1 ^ (h2 << 1);
    }
};

struct Node {
    std::pair<int, int> playerPos;
    std::set<std::pair<int, int>> boxes; // Using set for canonical state
    std::string path; 

    // For A*
    int cost = 0; // g(n)
    int heuristic = 0; // h(n)

    int f() const { return cost + heuristic; }

    // Operator for std::set (visited states) and sorting
    bool operator<(const Node& other) const {
        if (playerPos != other.playerPos)
            return playerPos < other.playerPos;
        return boxes < other.boxes;
    }

    bool operator==(const Node& other) const {
        return playerPos == other.playerPos && boxes == other.boxes;
    }
};

struct NodeHash {
    std::size_t operator()(const Node& n) const {
        std::size_t seed = 0;
        
        // Hash player pos
        auto hPlayer = pair_hash{}(n.playerPos);
        seed ^= hPlayer + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        // Hash boxes
        // Since std::set is ordered, we can just iterate and combine hashes
        for(const auto& box : n.boxes) {
            auto hBox = pair_hash{}(box);
            seed ^= hBox + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

struct NodeComparator {
    bool operator()(const Node& a, const Node& b) const {
        return a.f() > b.f(); // Min-priority queue for A*
    }
};

struct GreedyNodeComparator {
    bool operator()(const Node& a, const Node& b) const {
        // Pour le Greedy, on ne regarde que l'heuristique h(n)
        return a.heuristic > b.heuristic;
    }
};

#endif // NODE_H
