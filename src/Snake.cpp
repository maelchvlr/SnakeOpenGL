#include "Snake.h"
#include <iostream>

Snake::Snake(Grid& InGrid, Position pos) : currentDirection(Direction::RIGHT), grid(InGrid) {
	// Initialize the snake with a default position
    body.push_back(pos); // Example: Start at position (0, 0)
    positionQueues.push_back(std::queue<Position>());
    Position bodypart = pos;
    bodypart.z -= 1;
    body.push_back(bodypart);
    positionQueues.push_back(std::queue<Position>());


	// Update the grid with the initial snake position
	grid.setCellContent(0, 0, CellContent::Snake);
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

    if (grid.getCellContent(body.front().x + 10, body.front().z + 10) == CellContent::Pill)
    {
        grow();
	}

    if (grid.getCellContent(body.front().x + 10, body.front().z + 10) == CellContent::Obstacle)
    {
        std::cout << "Game Over" << std::endl;
	}

    for (auto& segment : body) {
		grid.setCellContent(segment.x + 10, segment.z + 10, CellContent::Snake);
	}

    
}