#include "Maze.h"
#include "GraphicAllegro5.h"
#include <fstream>
#include <exception>

Maze::Maze(const std::string& levelPath)
{
    std::vector<std::string> lines;
    std::string line;
    std::ifstream iss(levelPath);

    while (std::getline(iss, line))
    {
        lines.push_back(line);
        this->m_lig++;
        this->m_col = (this->m_col < line.size() ? line.size() : this->m_col);
    }

    this->m_field.resize(this->m_lig, std::vector<Square>(this->m_col));

    if (this->m_col <= NB_MAX_WIDTH && this->m_lig <= NB_MAX_HEIGHT)
    {
        for (unsigned int i=0; i<lines.size(); i++)
        {
            //std::cout << "Maze::load => Reading : " << lines[i] << std::endl;
            for (unsigned int j=0; j<this->m_col; j++)
            {
                if (j < lines[i].size())
                {
                    Square s;
                    s.position = std::make_pair(i, j);
                    s.sprite = (SpriteType)lines[i][j];
                    this->m_field[i][j] = s;

                    if (s.sprite == SpriteType::PLAYER || s.sprite == SpriteType::PLAYER_ON_GOAL)
                    {
                        this->m_playerPosition = std::make_pair(i, j);
                        if (s.sprite == SpriteType::PLAYER_ON_GOAL)
                        {
                            this->m_field[i][j].sprite = SpriteType::GOAL;
                        }
                    }
                }
                else
                {
                    // Here - Out of bound
                    Square s;
                    s.position = std::make_pair(i, j);
                    s.sprite = SpriteType::GROUND;
                    this->m_field[i][j] = s;
                }
            }
        }
    }
    else
    {
        throw std::out_of_range("Width or height too large !");
    }
    this->computeStaticDeadlocks();
}

bool Maze::isWall(const std::pair<int, int>& position) const
{
    if (this->m_field[position.first][position.second].sprite == SpriteType::WALL)
    {
        return true;
    }

    return false;
}

bool Maze::isBox(const std::pair<int, int>& position) const
{
    if (this->m_field[position.first][position.second].sprite == SpriteType::BOX
        || this->m_field[position.first][position.second].sprite == SpriteType::BOX_PLACED)
    {
        return true;
    }

    return false;
}

bool Maze::isGoal(const std::pair<int, int>& position) const
{
    if (this->m_field[position.first][position.second].sprite == SpriteType::GOAL
        || this->m_field[position.first][position.second].sprite == SpriteType::BOX_PLACED)
    {
        return true;
    }

    return false;
}

bool Maze::isFree(const std::pair<int, int>& position) const
{
    if (this->m_field[position.first][position.second].sprite == SpriteType::GROUND
        || this->m_field[position.first][position.second].sprite == SpriteType::GOAL)
    {
        return true;
    }

    return false;
}

bool Maze::isSolution() const
{
    // Check if all boxes are on goals
    for (unsigned int i=0; i<this->m_field.size(); ++i)
    {
        for (unsigned int j=0; j<this->m_field[i].size(); ++j)
        {
            const auto s = this->m_field[i][j];
            if (s.sprite == SpriteType::BOX)
            {
                // One box is not placed on a goal
                return false;
            }
        }
    }

    return true;
}

bool Maze::pushBox(const std::pair<int, int>& position, char dir)
{
    auto newPosition = position;
    newPosition.first += neighbours[dir].first;
    newPosition.second += neighbours[dir].second;

    // Can push ?
    if (this->isWall(newPosition))
        return false;

    if (!this->isFree(newPosition))
        return false;

    // Apply the push
    if (this->m_field[position.first][position.second].sprite == SpriteType::BOX_PLACED)
    {
        this->m_field[position.first][position.second].sprite = SpriteType::GOAL;
    }
    else
    {
        this->m_field[position.first][position.second].sprite = SpriteType::GROUND;
    }

    if (this->isGoal(newPosition))
    {
        // push a box on a goal
        this->m_field[newPosition.first][newPosition.second].sprite = SpriteType::BOX_PLACED;
    }
    else
    {
        this->m_field[newPosition.first][newPosition.second].sprite = SpriteType::BOX;
    }

    this->m_playerDirection = dir;

    return true;
}

bool Maze::updatePlayer(char dir)
{
    if (dir < 0 || dir > DIRECTION_MAX)
    {
        return false;
    }

    bool updatePlayer = false;
    auto newPositionPlayer = this->getPlayerPosition();
    newPositionPlayer.first += neighbours[dir].first;
    newPositionPlayer.second += neighbours[dir].second;

    // Check for walls
    if (this->isWall(newPositionPlayer))
        return false;

    if (this->isBox(newPositionPlayer) == false)
    {
        updatePlayer = true;
    }
    else if (this->pushBox(newPositionPlayer, dir)) // Can we push the box ?
    {
        updatePlayer = true;
    }

    // Move the player and check if the level is solved
    if (updatePlayer)
    {
        this->m_playerPosition = newPositionPlayer;
        this->m_playerDirection = dir;
        return (this->isSolution());
    }

    return false;
}

void Maze::playSolution(GraphicAllegro5& g, const std::vector<char>& movesSolution)
{
    for (const auto& move : movesSolution)
    {
        g.clear();
        this->updatePlayer(move);
        this->draw(g);
        g.display();
    }
}

std::vector<std::pair<int, int>> Maze::getBoxes() const
{
    std::vector<std::pair<int, int>> boxes;
    for (unsigned int i = 0; i < this->m_field.size(); ++i)
    {
        for (unsigned int j = 0; j < this->m_field[i].size(); ++j)
        {
            if (this->isBox({i, j}))
            {
                boxes.push_back({i, j});
            }
        }
    }
    return boxes;
}

std::vector<std::pair<int, int>> Maze::getGoals() const
{
    std::vector<std::pair<int, int>> goals;
    for (unsigned int i = 0; i < this->m_field.size(); ++i)
    {
        for (unsigned int j = 0; j < this->m_field[i].size(); ++j)
        {
            if (this->isGoal({i, j}))
            {
                goals.push_back({i, j});
            }
        }
    }
    return goals;
}

void Maze::computeStaticDeadlocks() {
    // Initialise la matrice à "false" partout
    m_deadlocks.assign(m_lig, std::vector<bool>(m_col, false));

    for (unsigned int i = 0; i < m_lig; ++i) {
        for (unsigned int j = 0; j < m_col; ++j) {
            // Un mur ou un objectif ne peut pas être une case morte
            if (isWall({i, j}) || isGoal({i, j})) continue;

            // Vérification des murs adjacents (Haut, Bas, Gauche, Droite)
            bool wT = isWall({i - 1, j});
            bool wB = isWall({i + 1, j});
            bool wL = isWall({i, j - 1});
            bool wR = isWall({i, j + 1});

            // Si la case est un coin (deux murs perpendiculaires)
            if ((wT && wL) || (wT && wR) || (wB && wL) || (wB && wR)) {
                m_deadlocks[i][j] = true;
            }
        }
    }
}

void Maze::draw(GraphicAllegro5& g) const {
    for (unsigned int i = 0; i < this->m_field.size(); ++i) {
        for (unsigned int j = 0; j < this->m_field[i].size(); ++j) {
            // 1. D'abord, on dessine le sol ou les deadlocks (en ROUGE) [cite: 121]
            if (m_deadlocks[i][j]) {
                g.drawRect(j, i, j + 1, i + 1, COLOR_RED, 1, true);
            }

            // 2. Ensuite, on dessine les sprites par-dessus
            const auto s = this->m_field[i][j];
            if (s.sprite == SpriteType::WALL) {
                g.drawT(g.getSprite(BITMAP_WALL), j, i);
            }
            else if (s.sprite == SpriteType::BOX_PLACED) {
                g.drawT(g.getSprite(BITMAP_BOX_PLACED), j, i);
            }
            else if (s.sprite == SpriteType::BOX) {
                g.drawT(g.getSprite(BITMAP_BOX), j, i);
            }
            else if (s.sprite == SpriteType::GOAL) {
                g.drawT(g.getSprite(BITMAP_GOAL), j, i);
            }
        }
    }
    // 3. Enfin le joueur
    g.drawT(g.getSpritePlayer(this->m_playerDirection), this->m_playerPosition.second, this->m_playerPosition.first);
}
