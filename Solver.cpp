#include "Solver.h"
#include <algorithm>
#include <cmath>

Solver::Solver(const Maze& maze) {
    m_maxRow = maze.getNbLines();
    m_maxCol = maze.getNbCols();
    m_startPlayer = maze.getPlayerPosition();

    // Initialise la matrice de deadlocks du Solver
    m_staticDeadlocks.assign(m_maxRow, std::vector<bool>(m_maxCol, false));

    for(unsigned int i=0; i<m_maxRow; ++i) {
        for(unsigned int j=0; j<m_maxCol; ++j) {
            if(maze.isWall({i, j})) {
                m_walls.push_back({(int)i, (int)j});
            }
            // Copie l'information de deadlock depuis le Maze
            if(maze.isDeadlock({(int)i, (int)j})) {
                m_staticDeadlocks[i][j] = true;
            }
        }
    }
    auto goalsVec = maze.getGoals();
    m_goals = goalsVec;

    auto boxesVec = maze.getBoxes();
    m_startBoxes.insert(boxesVec.begin(), boxesVec.end());
}

// Optimized isWall using the vector (can be improved)
bool Solver::isWall(const std::pair<int, int>& pos) const {
    for(const auto& w : m_walls) {
        if(w == pos) return true;
    }
    return false;
}

bool Solver::isGoal(const std::pair<int, int>& pos) const {
    for(const auto& g : m_goals) {
        if(g == pos) return true;
    }
    return false;
}

std::vector<Node> Solver::expand(const Node& current) const {
    std::vector<Node> successors;

    const std::vector<std::pair<int,int>> directions = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };

    const char moveChars[] = {0, 1, 2, 3};

    for(int i=0; i<4; ++i) {
        std::pair<int, int> newPos = {
            current.playerPos.first + directions[i].first,
            current.playerPos.second + directions[i].second
        };

        if(isWall(newPos)) continue;

        bool isPush = false;
        if(current.boxes.count(newPos)) {
            std::pair<int, int> newBoxPos = {
                newPos.first + directions[i].first,
                newPos.second + directions[i].second
            };

            if(isWall(newBoxPos)) continue;

            if(current.boxes.count(newBoxPos)) continue;

            //if(m_staticDeadlocks[newBoxPos.first][newBoxPos.second]) continue;

            isPush = true;

            Node nextNode = current;
            nextNode.playerPos = newPos;
            nextNode.boxes.erase(newPos);
            nextNode.boxes.insert(newBoxPos);
            nextNode.path.push_back(moveChars[i]);
            nextNode.cost = current.cost + 1;

            successors.push_back(nextNode);

        } else {
            // Just move
            Node nextNode = current;
            nextNode.playerPos = newPos;
            nextNode.path.push_back(moveChars[i]);
            nextNode.cost = current.cost + 1;
            successors.push_back(nextNode);
        }
    }

    return successors;
}

std::vector<char> Solver::solveBFS() {
    std::queue<Node> openSet;
    std::set<Node> visited;

    Node startNode;
    startNode.playerPos = m_startPlayer;
    startNode.boxes = m_startBoxes;

    openSet.push(startNode);
    visited.insert(startNode);

    int nodesExplored = 0;

    while(!openSet.empty()) {
        Node current = openSet.front();
        openSet.pop();
        nodesExplored++;

        // Check goal
        bool allOnGoal = true;
        for(const auto& box : current.boxes) {
            if(!isGoal(box)) {
                allOnGoal = false;
                break;
            }
        }
        if(allOnGoal) {
            std::cout << "BFS Found solution! Length: " << current.path.size()
                      << " Nodes explored: " << nodesExplored << std::endl;
            std::vector<char> result(current.path.begin(), current.path.end());
            return result;
        }

        std::vector<Node> nextNodes = expand(current);
        for(const auto& next : nextNodes) {
            if(visited.find(next) == visited.end()) {
                visited.insert(next);
                openSet.push(next);
            }
        }
    }

    std::cout << "BFS: No solution found." << std::endl;
    return {};
}

std::vector<char> Solver::solveDFS() {
    std::stack<Node> openSet;
    std::set<Node> visited;

    Node startNode;
    startNode.playerPos = m_startPlayer;
    startNode.boxes = m_startBoxes;

    openSet.push(startNode);
    visited.insert(startNode);

    int nodesExplored = 0;

    while(!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();
        nodesExplored++;

         // Check goal
        bool allOnGoal = true;
        for(const auto& box : current.boxes) {
            if(!isGoal(box)) {
                allOnGoal = false;
                break;
            }
        }
        if(allOnGoal) {
            std::cout << "DFS Found solution! Length: " << current.path.size()
                      << " Nodes explored: " << nodesExplored << std::endl;
            std::vector<char> result(current.path.begin(), current.path.end());
            return result;
        }

        std::vector<Node> nextNodes = expand(current);

        for(const auto& next : nextNodes) {
            if(visited.find(next) == visited.end()) {
                visited.insert(next);
                openSet.push(next);
            }
        }
    }
    return {};
}

int Solver::calculateHeuristic(const Node& node) const {
    int h = 0;
    for(const auto& box : node.boxes) {
        int minDist = 999999;
        for(const auto& goal : m_goals) {
            int d = std::abs(box.first - goal.first) + std::abs(box.second - goal.second);
            if(d < minDist) minDist = d;
        }
        h += minDist;
    }
    return h;
}

std::vector<char> Solver::solveAStar() {
    std::priority_queue<Node, std::vector<Node>, NodeComparator> openSet;
    std::set<Node> visited;

    Node startNode;
    startNode.playerPos = m_startPlayer;
    startNode.boxes = m_startBoxes;
    startNode.heuristic = calculateHeuristic(startNode);

    openSet.push(startNode);
    // Note: For A*, we should technically allow re-visiting if we find a cheaper path.
    // But with consistent heuristic and unit steps, a closed set is usually fine for simple graph search
    // UNLESS the heuristic is non-monotonic.
    // Here we'll stick to a simple visited set for simplicity first.
    visited.insert(startNode);

    int nodesExplored = 0;

    while(!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();
        nodesExplored++;

        // Check solution
        bool allOnGoal = true;
        for(const auto& box : current.boxes) {
            if(!isGoal(box)) {
                allOnGoal = false;
                break;
            }
        }
        if(allOnGoal) {
            std::cout << "A* Found solution! Length: " << current.path.size()
                      << " Nodes explored: " << nodesExplored << std::endl;
            std::vector<char> result(current.path.begin(), current.path.end());
            return result;
        }

        std::vector<Node> nextNodes = expand(current);
        for(auto& next : nextNodes) {
            if(visited.find(next) == visited.end()) {
                next.heuristic = calculateHeuristic(next);
                visited.insert(next);
                openSet.push(next);
            }
        }
    }
     std::cout << "A*: No solution found." << std::endl;
    return {};
}

std::vector<char> Solver::solveBruteForce() {
    Node startNode;
    startNode.playerPos = m_startPlayer;
    startNode.boxes = m_startBoxes;
    std::vector<char> solution;

    // On limite la profondeur à 15 ou 20 car sans 'visited',
    // l'algorithme s'arrêtera vite par manque de puissance[cite: 28, 117].
    int maxDepth = 15;
    std::cout << "Starting Brute Force (max depth: " << maxDepth << ")..." << std::endl;

    if (bruteForceRecursive(startNode, 0, maxDepth, solution)) {
        std::cout << "Brute Force: Solution found!" << std::endl;
        return solution;
    }

    std::cout << "Brute Force: No solution found." << std::endl;
    return {};
}

bool Solver::bruteForceRecursive(Node& current, int depth, int maxDepth, std::vector<char>& solution) {
    // 1. Vérification de la victoire [cite: 49, 97]
    bool allOnGoal = true;
    for(const auto& box : current.boxes) {
        if(!isGoal(box)) { allOnGoal = false; break; }
    }
    if(allOnGoal) return true;

    // 2. Limite de profondeur (évite de boucler à l'infini) [cite: 102]
    if (depth >= maxDepth) return false;

    // 3. Expansion des successeurs [cite: 98, 99]
    // L'ordre HAUT, BAS, GAUCHE, DROITE est respecté via expand [cite: 109, 111]
    std::vector<Node> successors = expand(current);

    for (const auto& next : successors) {
        // Ajouter le mouvement à la solution [cite: 117]
        solution.push_back(next.path.back());

        Node temp = next; // Copie de l'état actuel
        if (bruteForceRecursive(temp, depth + 1, maxDepth, solution)) {
            return true;
        }

        // Backtracking : on retire le mouvement si ce chemin ne mène à rien
        solution.pop_back();
    }

    return false;
}
