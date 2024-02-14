#include "Grid.h"
#include <iostream>

Grid::Grid(int width, int height) : width(width), height(height) {
    cells = std::vector<std::vector<CellContent>>(height, std::vector<CellContent>(width, CellContent::Empty));
    std::cout << "Grid initialized with size " << width << "x" << height << std::endl;
}

CellContent Grid::getCellContent(int x, int y) const {
    return cells[y][x];
}

void Grid::setCellContent(int x, int y, CellContent content) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        //std::cout << "Setting cell content at " << x << ", " << y << std::endl;
        //std::cout << "Previous content: " << static_cast<int>(cells[y][x]) << std::endl;
        cells[y][x] = content;
        //std::cout << "New content: " << static_cast<int>(cells[y][x]) << std::endl;
    }
    else {
        std::cerr << "Attempted to access grid out of bounds: " << x << ", " << y << std::endl;
    }
}

void Grid::placePill() {
    srand(time(nullptr)); // Seed for random number generation
    int x, y;
    do {
        x = rand() % getWidth();
        y = rand() % getHeight();
    } while (getCellContent(x, y) != CellContent::Empty);
    setCellContent(x, y, CellContent::Pill);
}
