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
#include <algorithm>
#include <map>


class Solver {
public:
    Solver(const Maze& m);

    // algos
    std::vector<char> solveBruteForce();
    std::vector<char> solveBFS();
    std::vector<char> solveDFS();

    std::vector<char> solveAStar();
    std::vector<char> solveBestFirst();

    // check deadlock dyn
    bool test_dl(const Node& n) const;

private:
    std::vector<std::pair<int, int>> w; // murs
    std::vector<std::pair<int, int>> g; // goals
    int h_max;
    int w_max;

    std::pair<int, int> p_init;
    std::set<std::pair<int, int>> b_init;

    // helpers
    bool check_w(const std::pair<int, int>& pos) const;
    bool check_g(const std::pair<int, int>& pos) const;
    
    // pour avoir les successeurs
    std::vector<Node> go(const Node& n) const;

    // heuristique
    int calc_h(const Node& n) const;

    std::vector<std::vector<bool>> dead;

    bool bf_rec(Node& cur, int prof, int max_prof, std::vector<char>& sol);
};

#endif // SOLVER_H
