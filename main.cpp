#include "Maze.h"
#include "GraphicAllegro5.h"
#include "Solver.h"

GraphicAllegro5 graphic(1024, 768);

std::vector<std::string> allLevels = {
    "levels/Demo.txt",
    "levels/Easy1.txt", "levels/Easy2.txt", "levels/Easy3.txt", "levels/Easy4.txt", "levels/Easy5.txt",
    "levels/Medium1.txt", "levels/Medium2.txt", "levels/Medium3.txt", "levels/Medium4.txt", "levels/Medium5.txt"
};
int currentIdx = 1; // On commence sur Easy1.txt

int main()
{
    Maze m(allLevels[currentIdx]);

    graphic.show();

    while (!graphic.keyGet(ALLEGRO_KEY_ESCAPE) && !graphic.keyGet(ALLEGRO_KEY_Q))
    {
        // Move player
        if (graphic.keyGet(ALLEGRO_KEY_UP)) m.updatePlayer(TOP);
        if (graphic.keyGet(ALLEGRO_KEY_DOWN)) m.updatePlayer(BOTTOM);
        if (graphic.keyGet(ALLEGRO_KEY_LEFT)) m.updatePlayer(LEFT);
        if (graphic.keyGet(ALLEGRO_KEY_RIGHT)) m.updatePlayer(RIGHT);

        // Charger le niveau SUIVANT (Touche N)
        if (graphic.keyGet(ALLEGRO_KEY_N)) {
            currentIdx = (currentIdx + 1) % allLevels.size();
            m = Maze(allLevels[currentIdx]); // Recharge l'objet Maze avec le nouveau fichier
            std::cout << "Niveau charge : " << allLevels[currentIdx] << std::endl;
        }

        // Charger le niveau PRÉCÉDENT (Touche V)
        if (graphic.keyGet(ALLEGRO_KEY_V)) {
            currentIdx = (currentIdx - 1 + allLevels.size()) % allLevels.size();
            m = Maze(allLevels[currentIdx]);
            std::cout << "Niveau charge : " << allLevels[currentIdx] << std::endl;
        }
        if (graphic.keyGet(ALLEGRO_KEY_C))
        {
            // Check if the level is solved
            if (m.isSolution()) { std::cout << "Level solved !" << std::endl; }
            else { std::cout << "Level not solved !" << std::endl; }
        }

        if (graphic.keyGet(ALLEGRO_KEY_P))
        {
            // Solution of the Easy1 level, just for test
            /// Attention : Il est interdit de "hard coder" les solutions des niveaux...
            const std::vector<char> moves = {BOTTOM, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, TOP, RIGHT, BOTTOM};
            m.playSolution(graphic, moves);
        }

        // Solver Triggers
        if (graphic.keyGet(ALLEGRO_KEY_F)) // BRUTE FORCE
        {
            std::cout << "Starting Brute Force..." << std::endl;
            Solver solver(m);
            auto solution = solver.solveBruteForce();
            if(!solution.empty()) m.playSolution(graphic, solution);
        }
        if (graphic.keyGet(ALLEGRO_KEY_B)) // BFS
        {
            std::cout << "Starting BFS..." << std::endl;
            Solver solver(m);
            auto solution = solver.solveBFS();
            if(!solution.empty()) m.playSolution(graphic, solution);
        }

        if (graphic.keyGet(ALLEGRO_KEY_D)) // DFS
        {
            std::cout << "Starting DFS..." << std::endl;
            Solver solver(m);
            auto solution = solver.solveDFS();
            if(!solution.empty()) m.playSolution(graphic, solution);
        }

        if (graphic.keyGet(ALLEGRO_KEY_A)) // A*
        {
            std::cout << "Starting A*..." << std::endl;
            Solver solver(m);
            auto solution = solver.solveAStar();
            if(!solution.empty()) m.playSolution(graphic, solution);
        }

        if (graphic.keyGet(ALLEGRO_KEY_R))
        {
            // Reload the level
            m = Maze(allLevels[currentIdx]);
        }

        // Display on screen (double buffer)
        graphic.clear();
        m.draw(graphic);
        graphic.display();
    }

    return 0;
}
