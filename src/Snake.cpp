#include "Snake.h"
#include "Grid.h"
#include <iostream>

Snake::Snake(Grid& InGrid, Position pos) : grid(InGrid) {
	// Initialize the snake with a default position
    body.push_back(pos); 
    positionQueues.push_back(std::queue<Position>());
    Position bodypart = pos;
    bodypart.z -= 1;
    body.push_back(bodypart);
    positionQueues.push_back(std::queue<Position>());


	// Update the grid with the initial snake position
	grid.setCellContent(0, 0, CellContent::Snake);

    currentDirection = Direction::DOWN; 
}

void Snake::move(Direction direction) {
    currentDirection = direction;
    Position Head = body.front();
    switch (currentDirection) {
    case Direction::UP:    Head.z -= 0.02f; break;
    case Direction::DOWN:  Head.z += 0.02f; break;
    case Direction::LEFT:  Head.x -= 0.02f; break;
    case Direction::RIGHT: Head.x += 0.02f; break;
    }
    if (Head.x < -10.0f || Head.x > 10.0f || Head.z < -10.0f || Head.z > 10.0f)
    {
		std::cout << "Game Over" << std::endl;
	}
    else
    {
        body.front() = Head; // Directly update the head position

        if (body.size() > 1 && positionQueues.size() > 1) {
            positionQueues[1].push(Head); // Corrected to push the new head position
        }

        for (size_t i = 1; i < body.size(); i++) {
            if (!positionQueues[i].empty() && positionQueues[i].size() > 50) {
                body[i] = positionQueues[i].front();
                positionQueues[i].pop();
                if (i + 1 < positionQueues.size()) { // Added check to prevent out-of-range access
                    positionQueues[i + 1].push(body[i]);
                }
            }
        }
        updateGrid();
    }
}

void Snake::grow()
{
    std::cout << "Growing snake" << std::endl;

    body.push_back(body.back());
    positionQueues.push_back(std::queue<Position>());

    grid.placePill();
    std::cout << "Pill placed" << std::endl;
}

void Snake::updateGrid() {
    for (int i = 0; i < grid.getHeight(); i++) {
        for (int j = 0; j < grid.getWidth(); j++) {
            if (grid.getCellContent(j, i) == CellContent::Snake) {
				grid.setCellContent(j, i, CellContent::Empty);
			}
		}
	}

    switch (currentDirection) {
        case Direction::UP:    offSetCorrection.z = -0.02; break;
        case Direction::DOWN:  offSetCorrection.z = -0.98; break;
        case Direction::LEFT:  offSetCorrection.x = -0.02; break;
        case Direction::RIGHT: offSetCorrection.x = -0.98; break;
    }

    if (grid.getCellContent(round(body.front().x + 10 + offSetCorrection.x), round(body.front().z + 10 + offSetCorrection.z)) == CellContent::Pill)
    {
        grow();
	}

    if (grid.getCellContent(round(body.front().x + 10 + offSetCorrection.x), round(body.front().z + 10 + offSetCorrection.z)) == CellContent::Obstacle)
    {
        std::cout << "Game Over" << std::endl;
	}

    for (auto& segment : body) {
		grid.setCellContent(round(segment.x + 10 + offSetCorrection.x), round(segment.z + 10 + offSetCorrection.z), CellContent::Snake);
	}

    
}

void Snake::calculateAndFollowPath() {
    if (gameOver) return;
    Position pillPosition = grid.getPillPosition(); 

    Grid::Node* startNode = grid.getNode(round(body.front().x + 10 + offSetCorrection.x), round(body.front().z + 10 + offSetCorrection.z)); // Convert Position to grid coordinates as needed
    
    Grid::Node* goalNode = grid.getNode(pillPosition.x, pillPosition.z);

    auto pathNodes = grid.findPath(*startNode, *goalNode);
    currentPath.clear();
    for (auto node : pathNodes) {
        currentPath.push_back(Position(node->x, node->y)); // Convert grid coordinates back to Position
    }

    followPath();
}

void Snake::GameOver()
{
    gameOver = true;
}


void Snake::followPath() {
    if (!currentPath.empty()) {
        Position nextStep = currentPath.front();
        currentPath.pop_front();

        // Determine direction based on the next step
        Position directionVector = Position((nextStep.x - round(body.front().x + 10 + offSetCorrection.x)),(nextStep.z - round(body.front().z + 10 + offSetCorrection.z)));
        

        if (directionVector.x > 0) currentDirection = Direction::RIGHT;
        else if (directionVector.x < 0) currentDirection = Direction::LEFT;
        else if (directionVector.z > 0) currentDirection = Direction::DOWN;
        else if (directionVector.z < 0) currentDirection = Direction::UP;


        // Move the snake in the determined direction
        move(currentDirection); 
    }
}

