#ifndef GRID_H
#define GRID_H

#include <vector>
#include "CellContent.h"

class Grid {
public:
    Grid() = default;
    Grid(int width, int height);

    CellContent getCellContent(int x, int y) const;
    void setCellContent(int x, int y, CellContent content);
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    std::vector<std::vector<CellContent>> cells;
    int width, height;
};

#endif // GRID_H