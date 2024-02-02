#ifndef GAME_H
#define GAME_H

#include <glew.h>
#include <glfw3.h>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Snake.h"
#include "Grid.h"

class Game {
public:
    Game();
    ~Game();
    void run();

    void screenPosToGridPos(double xpos, double ypos, int& gridX, int& gridY);
    void toggleObstacleAt(int gridX, int gridY);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void placePill();
    static Game* gameInstance; // Add a static pointer to the Game instance

private:
    GLFWwindow* window;
    GLuint VAO, VBO, shaderProgram;
    Snake snake;
    Grid grid;

    GLuint gridVAO, gridVBO; 
    const int gridSize = 10;
    glm::mat4 viewMatrix, projectionMatrix;


    void init();
    GLuint loadShader(const std::string& vertexPath, const std::string& fragmentPath);
    void setupCube();
    void update();
    void render();
    void cleanup();
    void setupGrid();

};

#endif // GAME_H
