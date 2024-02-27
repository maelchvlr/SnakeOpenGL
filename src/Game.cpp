#include "Game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


// Global variables for window dimensions
int windowWidth = 800;
int windowHeight = 600;

// Static instance pointer initialization to nullptr. It's used for singleton design pattern.
Game* Game::gameInstance = nullptr;

// Game constructor
Game::Game()
    : window(nullptr), VAO(0), VBO(0), shaderProgram(0),
    grid(20, 20), snake(grid, Position(0.5f, 0.5f)) { // Initializes the game with a window, a snake at origin, and a 20x20 grid

    gameInstance = this; // Sets the static instance pointer to this instance
    init(); // Initialize GLFW and GLEW, create window
    shaderProgram = loadShader("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl"); // Load and compile shaders
    setupGrid(); // Setup grid geometry
    setupCube(); // Setup cube geometry (used for obstacles and snake)
}

// Game destructor for cleanup
Game::~Game() {
    cleanup(); // Cleanup allocated resources
}

// Initializes GLFW, creates a window, and initializes GLEW
void Game::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        exit(-1); // Exit if GLFW initialization fails
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "Snake Game", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window\n";
        exit(-1); // Exit if window creation fails
    }

    // Set mouse button callback function
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    glfwMakeContextCurrent(window); // Make the window's context current

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Enable GLEW experimental features
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        exit(-1); // Exit if GLEW initialization fails
    }

    // enable depth testing
    glEnable(GL_DEPTH_TEST);
}

// Loads vertex and fragment shaders, compiles them, and links them into a shader program
GLuint Game::loadShader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    // Open and read the shader files
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vShaderFile.close();
    fShaderFile.close();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    // Convert shader source strings into character arrays
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile shaders
    GLuint vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    // Link shaders into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);

    // Delete shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shaderProgram; // Return the shader program ID
}

// Setup cube geometry for obstacles and snake body parts
void Game::setupCube() {
    float vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f, // Bottom-left
         0.5f,  0.5f, -0.5f, // Top-right
         0.5f, -0.5f, -0.5f, // Bottom-right         
         0.5f,  0.5f, -0.5f, // Top-right
        -0.5f, -0.5f, -0.5f, // Bottom-left
        -0.5f,  0.5f, -0.5f, // Top-left

        // Front face
        -0.5f, -0.5f,  0.5f, // Bottom-left
         0.5f, -0.5f,  0.5f, // Bottom-right
         0.5f,  0.5f,  0.5f, // Top-right
         0.5f,  0.5f,  0.5f, // Top-right
        -0.5f,  0.5f,  0.5f, // Top-left
        -0.5f, -0.5f,  0.5f, // Bottom-left

        // Left face
        -0.5f,  0.5f,  0.5f, // Top-right
        -0.5f,  0.5f, -0.5f, // Top-left
        -0.5f, -0.5f, -0.5f, // Bottom-left
        -0.5f, -0.5f, -0.5f, // Bottom-left
        -0.5f, -0.5f,  0.5f, // Bottom-right
        -0.5f,  0.5f,  0.5f, // Top-right

        // Right face
         0.5f,  0.5f,  0.5f, // Top-left
         0.5f, -0.5f, -0.5f, // Bottom-right
         0.5f,  0.5f, -0.5f, // Top-right         
         0.5f, -0.5f, -0.5f, // Bottom-right
         0.5f,  0.5f,  0.5f, // Top-left
         0.5f, -0.5f,  0.5f, // Bottom-left     

         // Bottom face
         -0.5f, -0.5f, -0.5f, // Top-right
          0.5f, -0.5f, -0.5f, // Top-left
          0.5f, -0.5f,  0.5f, // Bottom-left
          0.5f, -0.5f,  0.5f, // Bottom-left
         -0.5f, -0.5f,  0.5f, // Bottom-right
         -0.5f, -0.5f, -0.5f, // Top-right

         // Top face
         -0.5f,  0.5f, -0.5f, // Top-left
          0.5f,  0.5f,  0.5f, // Bottom-right
          0.5f,  0.5f, -0.5f, // Top-right     
          0.5f,  0.5f,  0.5f, // Bottom-right
         -0.5f,  0.5f, -0.5f, // Top-left
         -0.5f,  0.5f,  0.5f  // Bottom-left
    };

    // Generate and bind a Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Enable vertex attribute pointers for position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Main game loop
void Game::run() {

    float lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {

        // Calculate delta time
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        snake.calculateAndFollowPath();
        //snake.updateGrid();
        update();
        render();

        glfwSwapBuffers(window);
        glfwPollEvents(); // Poll for and process events
    }
}

// Update game logic
void Game::update() {
    

}

// Render the game
void Game::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up the view and projection matrices
    glUseProgram(shaderProgram);

    viewMatrix = glm::lookAt(glm::vec3(0.0f, 15.0f, 15.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projectionMatrix = glm::perspective(glm::radians(60.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Render the grid
    // set color to orange for grid
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 0.5f, 0.0f);

    glm::mat4 gridModel = glm::mat4(1.0f);  // Grid model matrix (identity matrix for no transformation)
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(gridModel));
    glBindVertexArray(gridVAO); 
    glDrawArrays(GL_LINES, 0, (gridSize * 2 + 1) * 4);
    glBindVertexArray(0);

    // Render the snake
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 1.0f, 0.0f); // Set color to green for snake
    for (auto& pos : snake.getBody()) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos.toVec3());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
	}

    // Render the obstacles and pills
    float cellSize = 1.0f; // Size of each cell in the grid
    
    // Adjust for the center of the cell
    float offsetX = (gridSize * 2.0f) / grid.getWidth() / 2.0f; // Half of cell width
    float offsetY = 0.5f; // place the cube on top of the grid
    float offsetZ = (gridSize * 2.0f) / grid.getHeight() / 2.0f; // Half of cell depth

    for (int x = 0; x < grid.getWidth(); ++x) {
        for (int y = 0; y < grid.getHeight(); ++y) {
            if (grid.getCellContent(x, y) == CellContent::Obstacle) {

                glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 0.0f, 0.0f); // Set color to red for obstacles
                // Calculate world position with center offset
                glm::vec3 worldPos = glm::vec3(
                    (x - grid.getWidth() / 2.0f) * cellSize + offsetX,
                    offsetY,
                    (y - grid.getHeight() / 2.0f) * cellSize + offsetZ
                );

                glm::mat4 model = glm::translate(glm::mat4(1.0f), worldPos);
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if (grid.getCellContent(x, y) == CellContent::Pill) {
                glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0.0f, 0.0f, 1.0f); // Set color to blue for pill

                // Calculate world position with center offset
                glm::vec3 worldPos = glm::vec3(
                    (x - grid.getWidth() / 2.0f) * cellSize + offsetX,
                    offsetY,
                    (y - grid.getHeight() / 2.0f) * cellSize + offsetZ
                );

                // Render the pill
                glm::mat4 model = glm::translate(glm::mat4(1.0f), worldPos);

                // Apply scaling transformation to make the pill smaller
                // Adjust the glm::vec3 values to scale the pill to the desired size
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); // Example: scale down to half size

                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if(grid.getCellContent(x,y) == CellContent::Path){
				glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 1.0f); // Set color to cyan for path
				// Calculate world position with center offset
				glm::vec3 worldPos = glm::vec3(
					(x - grid.getWidth() / 2.0f) * cellSize + offsetX,
					offsetY,
					(y - grid.getHeight() / 2.0f) * cellSize + offsetZ
				);

				glm::mat4 model = glm::translate(glm::mat4(1.0f), worldPos);

                model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); // Example: scale down to half size
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
            
			
        }
    }


}


// Cleanup allocated resources: Called when closing the game to properly free resources.
void Game::cleanup() {
    glDeleteVertexArrays(1, &VAO); // Delete the Vertex Array Object.
    glDeleteBuffers(1, &VBO); // Delete the Vertex Buffer Object.
    glDeleteProgram(shaderProgram); // Delete the shader program.
    glfwDestroyWindow(window); // Destroy the GLFW window.
    glfwTerminate(); // Terminate GLFW.
}

// Setup grid: Initializes the geometry for drawing the grid on the screen.
void Game::setupGrid() {
    std::vector<float> gridVertices; // Store the vertices of the grid lines.
    // Create vertices for vertical and horizontal lines of the grid.
    for (int i = -gridSize; i <= gridSize; ++i) {
        gridVertices.push_back((float)i); gridVertices.push_back(0.0f); gridVertices.push_back((float)-gridSize); // Start of vertical line.
        gridVertices.push_back((float)i); gridVertices.push_back(0.0f); gridVertices.push_back((float)gridSize); // End of vertical line.
        gridVertices.push_back((float)-gridSize); gridVertices.push_back(0.0f); gridVertices.push_back((float)i); // Start of horizontal line.
        gridVertices.push_back((float)gridSize); gridVertices.push_back(0.0f); gridVertices.push_back((float)i); // End of horizontal line.
    }

    // Extra lines to close the grid visually.
    float lastLine = gridSize + 1;
    gridVertices.push_back(lastLine); gridVertices.push_back(0.0f); gridVertices.push_back((float)-gridSize);
    gridVertices.push_back(lastLine); gridVertices.push_back(0.0f); gridVertices.push_back((float)gridSize);
    gridVertices.push_back((float)-gridSize); gridVertices.push_back(0.0f); gridVertices.push_back(lastLine);
    gridVertices.push_back((float)gridSize); gridVertices.push_back(0.0f); gridVertices.push_back(lastLine);

    // Generate and bind the VAO and VBO for the grid.
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), &gridVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


// Converts screen positions to grid positions
void Game::screenPosToGridPos(double xpos, double ypos, int& gridX, int& gridY) {
    // First, convert screen position (xpos, ypos) to normalized device coordinates (NDC)
    // This transformation maps the screen position to a range between -1 and 1 for both axes
    glm::vec4 ray_clip = glm::vec4((2.0f * xpos) / windowWidth - 1.0f, 1.0f - (2.0f * ypos) / windowHeight, -1.0, 1.0);

    // Convert from NDC to eye coordinates
    // This step involves reversing the projection transformation applied during rendering
    glm::vec4 ray_eye = glm::inverse(projectionMatrix) * ray_clip;
    ray_eye.z = -1.0; // Set the direction of the ray into the scene
    ray_eye.w = 0.0; // This ensures we get a direction vector, not a point

    // Convert from eye coordinates to world coordinates
    // Here we reverse the view transformation to get the direction of the ray in the world
    glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(viewMatrix) * ray_eye));

    // Use the camera's position as defined in the view matrix setup
    glm::vec3 cameraPos = glm::vec3(0.0f, 15.0f, 15.0f);

    // Find where the ray intersects with the ground plane (y=0)
    // This involves some basic ray-plane intersection math
    float t = (-cameraPos.y) / ray_wor.y;
    glm::vec3 intersection_point = cameraPos + ray_wor * t;

    // Map the intersection point to grid coordinates
    // This step converts the world position of the intersection to an index in the grid array
    float worldGridSize = gridSize * 2.0f; // Total size of the grid in world units
    glm::vec3 gridOrigin = glm::vec3(-gridSize, 0.0f, -gridSize); // World position of the grid's bottom-left corner

    // Calculate the intersection's position relative to the grid's origin
    glm::vec3 normalizedIntersection = intersection_point - gridOrigin;

    // Determine the grid cell by dividing the world units by the cell size (assumed to be 1x1 here)
    gridX = static_cast<int>(normalizedIntersection.x);
    gridY = static_cast<int>(normalizedIntersection.z);
}

// Toggles an obstacle's presence at the specified grid cell
void Game::toggleObstacleAt(int gridX, int gridY) {
    // Check if the specified grid coordinates are within the bounds of the grid
    if (gridX >= 0 && gridX < grid.getWidth() && gridY >= 0 && gridY < grid.getHeight()) {
        // Retrieve the content of the specified cell
        CellContent content = grid.getCellContent(gridX, gridY);

        // Toggle the cell content between empty and obstacle
        if (content == CellContent::Obstacle) {
            grid.setCellContent(gridX, gridY, CellContent::Empty);
        }
        else if (content == CellContent::Pill) {
			grid.setCellContent(gridX, gridY, CellContent::Obstacle);
			placePill();
		}
        else if (content == CellContent::Snake) {
            snake.GameOver();
		}
        else {
            grid.setCellContent(gridX, gridY, CellContent::Obstacle);
        }
        snake.calculateAndFollowPath();
    }
    else {
        // Log an error if the coordinates are out of bounds
        std::cout << "Attempted to access grid out of bounds: " << gridX << ", " << gridY << std::endl;
    }
}

// Handles mouse button press events
void Game::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Check if the left mouse button was pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

        // Get the current mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Initialize variables to store the calculated grid coordinates
        int gridX, gridY;

        // Convert the mouse position to grid coordinates
        gameInstance->screenPosToGridPos(xpos, ypos, gridX, gridY);

        // Toggle the obstacle at the calculated grid position
        gameInstance->toggleObstacleAt(gridX, gridY);
    }
}

void Game::placePill() {
    grid.placePill();
}