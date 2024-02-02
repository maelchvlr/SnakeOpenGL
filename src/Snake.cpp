#include "Snake.h"

Snake::Snake(const glm::vec3& startPosition) : position(startPosition), direction(0.0f, 0.0f, -1.0f), speed(2.0f) {
}

void Snake::update(float deltaTime) {
    //position += direction * speed * deltaTime;
}

glm::vec3 Snake::getPosition() const {
    return position;
}
