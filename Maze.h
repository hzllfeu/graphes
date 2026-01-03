#ifndef MAZE_H_INCLUDED
#define MAZE_H_INCLUDED

#include <string>
#include <vector>
#include "GraphicAllegro5.h"

// Max size for the field
#define NB_MAX_WIDTH     100
#define NB_MAX_HEIGHT    100

enum class SpriteType : unsigned char
{
    GROUND = ' ', OUTSIDE = 'X',
    WALL = '#', PLAYER = '@', PLAYER_ON_GOAL = '+',
    BOX = '$', BOX_PLACED = '*', GOAL = '.'
};

const std::vector<std::pair<int,int>> neighbours = {
    {-1, 0}, // N (top)
    {1, 0}, // S (bottom)
    {0, -1}, // O (left)
    {0, 1}, // E (right)
};

class GraphicAllegro5;

enum Direction
{
    NONE = -1,
    TOP = 0, BOTTOM = 1, LEFT = 2, RIGHT = 3,
    TOP_LEFT = 4, TOP_RIGHT = 5, BOTTOM_LEFT = 6, BOTTOM_RIGHT = 7,
    DIRECTION_MAX = 4, DIRECTION_DIAGO_MAX = 8,
};

struct Square
{
    std::pair<int, int> position;
    SpriteType sprite;
};

class Maze
{
    private:
        std::vector<std::vector<Square>> m_field;
        std::pair<int, int> m_playerPosition;
        std::vector<std::vector<bool>> m_deadlocks;
        unsigned int m_lig = 0, m_col = 0; // size of field
        char m_playerDirection = RIGHT;


    public:

        Maze(const std::string& levelPath);
        ~Maze() = default;

        bool updatePlayer(char dir);

        bool isWall(const std::pair<int, int>& position) const;
        bool isBox(const std::pair<int, int>& position) const;
        bool isGoal(const std::pair<int, int>& position) const;
        bool isFree(const std::pair<int, int>& position) const;

        std::vector<std::pair<int, int>> getBoxes() const;
        std::vector<std::pair<int, int>> getGoals() const;

        bool pushBox(const std::pair<int, int>& position, char dir);

        bool isSolution() const;
        void generate(int nbLig, int nbCol);

        void playSolution(GraphicAllegro5& g, const std::vector<char>& movesSolution);
        void draw(GraphicAllegro5& g) const;

        unsigned int getNbLines() const { return this->m_lig; }
        unsigned int getNbCols() const { return this->m_col; }

        const std::pair<int, int>& getPlayerPosition() const { return this->m_playerPosition; }

        void computeStaticDeadlocks();
        bool isDeadlock(const std::pair<int, int>& pos) const {
            return m_deadlocks[pos.first][pos.second];
        }
};

#endif // MAZE_H_INCLUDED
