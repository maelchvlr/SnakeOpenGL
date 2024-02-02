#ifndef SNAKE_H
#define SNAKE_H

#include <glm.hpp>

class Snake {
public:
    Snake(const glm::vec3& startPosition);

    void update(float deltatime);
    glm::vec3 getPosition() const;

private:
    glm::vec3 position;
    glm::vec3 direction;
    float speed;

    // Additional attributes like direction, speed, etc., can be added here.
};

#endif // SNAKE_H