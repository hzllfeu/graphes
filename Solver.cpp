#include "Solver.h"
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <chrono>

Solver::Solver(const Maze& m) {
    h_max = m.getNbLines();
    w_max = m.getNbCols();
    p_init = m.getPlayerPosition();

    // init mat
    dead.assign(h_max, std::vector<bool>(w_max, false));

    for(unsigned int i=0; i<h_max; ++i) {
        for(unsigned int j=0; j<w_max; ++j) {
            if(m.isWall({i, j})) {
                w.push_back({(int)i, (int)j});
            }
            // copie du truc
            if(m.isDeadlock({(int)i, (int)j})) {
                dead[i][j] = true;
            }
        }
    }
    auto tmp = m.getGoals();
    g = tmp;

    auto b = m.getBoxes();
    b_init.insert(b.begin(), b.end());
}

bool Solver::check_w(const std::pair<int, int>& pos) const {
    for(const auto& x : w) {
        if(x == pos) return true;
    }
    return false;
}

bool Solver::check_g(const std::pair<int, int>& pos) const {
    for(const auto& x : g) {
        if(x == pos) return true;
    }
    return false;
}

std::vector<Node> Solver::go(const Node& n) const {
    std::vector<Node> res;

    // up down etc
    const std::vector<std::pair<int,int>> d = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };

    const char c[] = {0, 1, 2, 3};

    for(int i=0; i<4; ++i) {
        std::pair<int, int> np = {
            n.playerPos.first + d[i].first,
            n.playerPos.second + d[i].second
        };

        if(check_w(np)) continue;

        bool push = false;
        if(n.boxes.count(np)) {
            std::pair<int, int> nbp = {
                np.first + d[i].first,
                np.second + d[i].second
            };

            if(check_w(nbp)) continue;
            if(n.boxes.count(nbp)) continue; // deja une caisse
            if(dead[nbp.first][nbp.second]) continue; // coin

            push = true;

            Node nx = n;
            nx.playerPos = np;
            nx.boxes.erase(np);
            nx.boxes.insert(nbp);
            nx.path.push_back(c[i]);
            nx.cost = n.cost + 1;

            res.push_back(nx);

        } else {
            // bouge simple
            Node nx = n;
            nx.playerPos = np;
            nx.path.push_back(c[i]);
            nx.cost = n.cost + 1;
            res.push_back(nx);
        }
    }

    return res;
}

std::vector<char> Solver::solveBFS() {
    auto t1 = std::chrono::high_resolution_clock::now();
    std::queue<Node> q;
    std::unordered_set<Node, NodeHash> vu;

    Node s;
    s.playerPos = p_init;
    s.boxes = b_init;

    q.push(s);
    vu.insert(s);

    int nb = 0;

    while(!q.empty()) {
        Node curr = q.front();
        q.pop();
        nb++;

        // win ?
        bool ok = true;
        for(const auto& b : curr.boxes) {
            if(!check_g(b)) {
                ok = false;
                break;
            }
        }
        if(ok) {
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = t2 - t1;
            std::cout << "BFS trouve : " << curr.path.size()
                      << " noeuds: " << nb
                      << " temps: " << diff.count() << "s" << std::endl;
            std::vector<char> r(curr.path.begin(), curr.path.end());
            return r;
        }

        std::vector<Node> next = go(curr);
        for(const auto& x : next) {
            if(vu.find(x) == vu.end()) {
                vu.insert(x);
                q.push(x);
            }
        }
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = t2 - t1;
    std::cout << "BFS rate... " << diff.count() << "s" << std::endl;
    return {};
}

std::vector<char> Solver::solveDFS() {
    auto t1 = std::chrono::high_resolution_clock::now();
    std::stack<Node> pile;
    std::unordered_set<Node, NodeHash> vu;

    Node s;
    s.playerPos = p_init;
    s.boxes = b_init;

    pile.push(s);
    vu.insert(s);

    int nb = 0;

    while(!pile.empty()) {
        Node curr = pile.top();
        pile.pop();
        nb++;

         // win ?
        bool ok = true;
        for(const auto& b : curr.boxes) {
            if(!check_g(b)) {
                ok = false;
                break;
            }
        }
        if(ok) {
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = t2 - t1;
            std::cout << "DFS trouve : " << curr.path.size()
                      << " noeuds: " << nb
                      << " temps: " << diff.count() << "s" << std::endl;
            std::vector<char> r(curr.path.begin(), curr.path.end());
            return r;
        }

        std::vector<Node> next = go(curr);

        for(const auto& x : next) {
            if(vu.find(x) == vu.end()) {
                vu.insert(x);
                pile.push(x);
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = t2 - t1;
    std::cout << "DFS rate... " << diff.count() << "s" << std::endl;
    return {};
}

int Solver::calc_h(const Node& n) const {
    int val = 0;
    for(const auto& b : n.boxes) {
        int min_d = 999999;
        for(const auto& gl : g) {
            int d = std::abs(b.first - gl.first) + std::abs(b.second - gl.second);
            if(d < min_d) min_d = d;
        }
        val += min_d;
    }
    return val;
}

std::vector<char> Solver::solveAStar() {
    auto t1 = std::chrono::high_resolution_clock::now();
    std::priority_queue<Node, std::vector<Node>, NodeComparator> q;
    std::unordered_set<Node, NodeHash> vu;

    Node s;
    s.playerPos = p_init;
    s.boxes = b_init;
    s.heuristic = calc_h(s); // h

    q.push(s);
    vu.insert(s);

    int nb = 0;

    while(!q.empty()) {
        Node curr = q.top();
        q.pop();
        nb++;

        // win ?
        bool ok = true;
        for(const auto& b : curr.boxes) {
            if(!check_g(b)) {
                ok = false;
                break;
            }
        }
        if(ok) {
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = t2 - t1;
            std::cout << "A* trouve ! len: " << curr.path.size()
                      << " noeuds: " << nb
                      << " t: " << diff.count() << "s" << std::endl;
            return std::vector<char>(curr.path.begin(), curr.path.end());
        }

        std::vector<Node> next = go(curr);
        for(auto& x : next) {
             // check deadlock dyn pour le lvl 3
            if (test_dl(x)) {
                continue; // mort
            }

            if(vu.find(x) == vu.end()) {
                x.heuristic = calc_h(x);
                vu.insert(x);
                q.push(x);
            }
        }
    }
     auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = t2 - t1;
    std::cout << "A* rate... " << diff.count() << "s" << std::endl;
    return {};
}

// Fonction pour voir si c mort (carre de 2x2)
bool Solver::test_dl(const Node& n) const {
    for (const auto& b : n.boxes) {
        if (check_g(b)) continue; // c bon

        int r = b.first;
        int c = b.second;

        // check autour
        auto chk = [&](int rr, int cc) { 
             if (rr == r && cc == c) return true; // moi
             if (check_w({rr, cc})) return true; // mur
             if (n.boxes.count({rr, cc})) return true; // caisse
             return false;
        };

        // les 4 carres
        if (chk(r-1, c-1) && chk(r-1, c) && chk(r, c-1)) return true;
        if (chk(r-1, c) && chk(r-1, c+1) && chk(r, c+1)) return true;
        if (chk(r, c-1) && chk(r+1, c-1) && chk(r+1, c)) return true;
        if (chk(r, c+1) && chk(r+1, c) && chk(r+1, c+1)) return true;
    }
    return false;
}

std::vector<char> Solver::solveBruteForce() {
    auto t1 = std::chrono::high_resolution_clock::now();
    Node s;
    s.playerPos = p_init;
    s.boxes = b_init;
    std::vector<char> sol;

    int max = 15;
    std::cout << "Brute Force (max " << max << ")..." << std::endl;

    if (bf_rec(s, 0, max, sol)) {
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = t2 - t1;
        std::cout << "BF trouve ! t: " << diff.count() << "s" << std::endl;
        return sol;
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = t2 - t1;
    std::cout << "BF rate... " << diff.count() << "s" << std::endl;
    return {};
}

bool Solver::bf_rec(Node& cur, int p, int max, std::vector<char>& sol) {
    // 1. win ?
    bool ok = true;
    for(const auto& b : cur.boxes) {
        if(!check_g(b)) { ok = false; break; }
    }
    if(ok) return true;

    // 2. prof max
    if (p >= max) return false;

    // 3. suite
    std::vector<Node> next = go(cur);

    for (const auto& n : next) {
        sol.push_back(n.path.back());
        Node tmp = n; 
        if (bf_rec(tmp, p + 1, max, sol)) {
            return true;
        }
        // backtrack
        sol.pop_back();
    }

    return false;
}

std::vector<char> Solver::solveBestFirst() {
    auto t1 = std::chrono::high_resolution_clock::now();
    // greedy
    std::priority_queue<Node, std::vector<Node>, GreedyNodeComparator> q;
    std::unordered_set<Node, NodeHash> vu;

    Node s;
    s.playerPos = p_init;
    s.boxes = b_init;
    s.heuristic = calc_h(s);

    q.push(s);
    vu.insert(s);

    int nb = 0;
    while(!q.empty()) {
        Node curr = q.top();
        q.pop();
        nb++;

        // win ?
        bool ok = true;
        for(const auto& b : curr.boxes) {
            if(!check_g(b)) { ok = false; break; }
        }
        if(ok) {
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = t2 - t1;
            std::cout << "Greedy trouve ! len: " << curr.path.size()
                      << " noeuds: " << nb
                      << " t: " << diff.count() << "s" << std::endl;
            return std::vector<char>(curr.path.begin(), curr.path.end());
        }

        std::vector<Node> tmp = go(curr);
        for(auto& x : tmp) {
            if(vu.find(x) == vu.end()) {
                x.heuristic = calc_h(x);
                vu.insert(x);
                q.push(x);
            }
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = t2 - t1;
    std::cout << "Greedy rate... t: " << diff.count() << "s" << std::endl;
    return {};
}
