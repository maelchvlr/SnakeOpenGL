#pragma once
#include <vector>
#include "glm.hpp"
#include <queue>
#include <list>

enum class Direction { UP, DOWN, LEFT, RIGHT };

class Grid;

struct Position {
    Position(float x, float z) : x(x), z(z) {}
    Position() : x(0), z(0) {}
    float x, z;
    float y = 0.5; // Default y position for the snake
    glm::vec3 toVec3() { return glm::vec3(x, y, z); }
};

class Snake {
public:
    Snake(Grid& InGrid, Position pos);
    void move(Direction direction);
    void grow();
    void updateGrid();
    void calculateAndFollowPath();
    std::vector<Position>& getBody() { return body; }
    void GameOver();

private:
    Grid& grid;
    std::vector<Position> body;
    std::vector<std::queue<Position>> positionQueues;
    std::list<Position> currentPath; // Stores the current path to the pill
    void followPath(); // Follows the calculated path
    
    bool gameOver = false;
    Direction currentDirection;
    Position offSetCorrection;
};