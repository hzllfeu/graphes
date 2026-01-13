#ifndef NODE_H
#define NODE_H

#include <vector>
#include <set>
#include <string>
#include <utility>
#include <functional>


struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
    }
};

struct Node {
    std::pair<int, int> playerPos;
    std::set<std::pair<int, int>> boxes; // On garde le set pour pas que �a bug
    std::string path;

    // g(n) et h(n)
    int cost = 0;
    int heuristic = 0;

    // calcul de f direct
    int f() const { return cost + heuristic; }

    // Pour comparer deux nodes dans le set des vues
    bool operator<(const Node& other) const {
        if (playerPos != other.playerPos)
            return playerPos < other.playerPos;
        return boxes < other.boxes;
    }

    // Pour tester l'�galit�
    bool operator==(const Node& other) const {
        return playerPos == other.playerPos && boxes == other.boxes;
    }
};

struct NodeHash {
    std::size_t operator()(const Node& n) const {
        std::size_t seed = 0;

        // 1. Hacher la position du joueur (avec la formule robuste)
        auto hPlayer = pair_hash{}(n.playerPos);
        seed ^= hPlayer + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        // 2. Hacher les boites
        for(const auto& box : n.boxes) {
            auto hBox = pair_hash{}(box);
            // C'est cette ligne qui fait toute la différence :
            seed ^= hBox + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

// Comparateur pour les priority queue
struct NodeComparator {
    bool operator()(const Node& a, const Node& b) const {
        return a.f() > b.f(); // A* : le plus petit f en premier
    }
};

struct GreedyNodeComparator {
    bool operator()(const Node& a, const Node& b) const {
        return a.heuristic > b.heuristic; // Greedy : on regarde que h
    }
};

#endif // NODE_H
