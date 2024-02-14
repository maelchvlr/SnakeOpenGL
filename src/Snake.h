#pragma once
#include <vector>
#include "Grid.h"
#include "glm.hpp"
#include <queue>

enum class Direction { UP, DOWN, LEFT, RIGHT };

struct Position {
    Position(float x, float z) : x(x), z(z) {}
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
    std::vector<Position>& getBody() { return body; }

private:
    Grid& grid;
    std::vector<Position> body;
    std::vector<std::queue<Position>> positionQueues;
    Direction currentDirection;
};