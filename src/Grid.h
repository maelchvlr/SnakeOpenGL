#ifndef GRID_H
#define GRID_H

#include <vector>
#include "CellContent.h"
#include <limits>
#include "Snake.h"

class Grid {
public:
    struct Node {
        bool walkable;
        int gCost, hCost;
        Node* parent;
        int x, y; // Position in the grid

        Node(bool walkable = true, int x = 0, int y = 0) : walkable(walkable), gCost(std::numeric_limits<int>::max()), hCost(0), parent(nullptr), x(x), y(y) {}

        int fCost() const { return gCost + hCost; }
    };

    Grid() = default;
    Grid(int width, int height);
    void initializeNodes(); // Initialize the nodes based on cells

    CellContent getCellContent(int x, int y) const;
    void setCellContent(int x, int y, CellContent content);
    Node* getNode(int x, int y); // Get the node at a specific position
    std::vector<Node*> getNeighbors(Node* node); // Get neighbors of a node
    void placePill();
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    Position getPillPosition() const;

    std::vector<Node*> findPath(const Node& start, const Node& goal);

private:

    int distanceBetweenNodes(Node* a, Node* b) const; // Helper method for A*
    std::vector<std::vector<CellContent>> cells;
    std::vector<std::vector<Node>> nodes; // Added nodes representation
    int width, height;
};

#endif // GRID_H