#include "Grid.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

Grid::Grid(int width, int height) : width(width), height(height) {
    cells = std::vector<std::vector<CellContent>>(height, std::vector<CellContent>(width, CellContent::Empty));
    initializeNodes();
    placePill();
    std::cout << "Grid initialized with size " << width << "x" << height << std::endl;
}

void Grid::initializeNodes() {
    nodes.resize(height, std::vector<Node>(width));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            nodes[y][x] = Node(cells[y][x] != CellContent::Obstacle && cells[y][x] != CellContent::Snake, x, y);
        }
    }
}

Grid::Node* Grid::getNode(int x, int y) {
    // Ensure coordinates are rounded or adjusted to fit the grid's indexing system
    // This might involve rounding x and y if they're floating-point in the snake's position
    int gridX = std::round(x);
    int gridY = std::round(y);

    if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
        return &nodes[gridY][gridX];
    }
    return nullptr;
}

std::vector<Grid::Node*> Grid::getNeighbors(Node* node) {
    std::vector<Node*> neighbors;

    // Positions of potential neighbors (left, right, above, below)
    std::vector<std::pair<int, int>> offsets = {
        {-1, 0}, // left
        {1, 0},  // right
        {0, -1}, // above
        {0, 1}   // below
    };

    for (auto& offset : offsets) {
        int neighborX = node->x + offset.first;
        int neighborY = node->y + offset.second;

        // Check if the neighbor is within grid bounds
        if (neighborX >= 0 && neighborX < width && neighborY >= 0 && neighborY < height) {
            neighbors.push_back(&nodes[neighborY][neighborX]);
        }
    }

    return neighbors;
}

CellContent Grid::getCellContent(int x, int y) const
{
    if (x >= 0 && x < width && y >= 0 && y < height) {
		return cells[y][x];
	}
	return CellContent::Obstacle;
}

void Grid::setCellContent(int x, int y, CellContent content) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        cells[y][x] = content;
        nodes[y][x].walkable = (content != CellContent::Obstacle && content != CellContent::Snake);
    }
    else {
        std::cerr << "Attempted to access grid out of bounds: " << x << ", " << y << std::endl;
    }
}

void Grid::placePill() {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed for random number generation
    int x, y;
    do {
        x = rand() % getWidth();
        y = rand() % getHeight();
    } while (getCellContent(x, y) != CellContent::Empty);
    setCellContent(x, y, CellContent::Pill);
}

int Grid::distanceBetweenNodes(Node* a, Node* b) const {
    // Using Manhattan distance as the heuristic
    int distanceX = std::abs(a->x - b->x);
    int distanceY = std::abs(a->y - b->y);
    return distanceX + distanceY;
}


Position Grid::getPillPosition() const
{
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            
            if (cells[y][x] == CellContent::Pill) {
				return Position(x, y);
			}
		}
	}
	return Position(-1, -1);
}

std::vector<Grid::Node*> Grid::findPath(const Node& start, const Node& goal) {
    Node* startNode = getNode(start.x, start.y);
    Node* goalNode = getNode(goal.x, goal.y);

    std::vector<Node*> openSet;
    std::vector<Node*> closedSet;
    openSet.push_back(startNode);

    while (!openSet.empty()) {
        Node* currentNode = openSet[0];
        for (int i = 1; i < openSet.size(); i++) {
            if (openSet[i]->fCost() < currentNode->fCost() || (openSet[i]->fCost() == currentNode->fCost() && openSet[i]->hCost < currentNode->hCost)) {
                currentNode = openSet[i];
            }
        }

        openSet.erase(std::remove(openSet.begin(), openSet.end(), currentNode), openSet.end());
        closedSet.push_back(currentNode);

        if (currentNode == goalNode) {
            std::vector<Node*> path;
            while (currentNode != nullptr && currentNode != startNode) {
                path.push_back(currentNode);
                currentNode = currentNode->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (Node* neighbor : getNeighbors(currentNode)) {
            if (std::find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end() || !neighbor->walkable) {
                continue;
            }

            int newCostToNeighbor = currentNode->gCost + distanceBetweenNodes(currentNode, neighbor);
            if (newCostToNeighbor < neighbor->gCost || std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end()) {
                neighbor->gCost = newCostToNeighbor;
                neighbor->hCost = distanceBetweenNodes(neighbor, goalNode);
                neighbor->parent = currentNode;

                if (std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end()) {
                    openSet.push_back(neighbor);
                }
            }
        }
    }

    return {}; // Return empty path if no path is found
}