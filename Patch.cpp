#include "Patch.h"
#include "Constants.h"
#include <algorithm>

Patch::Patch(float c, int f, bool n, float s) : chemical(c), food(f), nest(n), nestScent(s) {}

void Patch::getNeighbours(Patch patches[][DISPLAY_SIZE]) {
    std::vector<Patch *> neighbours;

    // Define the range of neighboring patches
    int minRow = std::max(0, x - 1);
    int maxRow = std::min(DISPLAY_SIZE - 1, x + 1);
    int minCol = std::max(0, y - 1);
    int maxCol = std::min(DISPLAY_SIZE - 1, y + 1);

    // Iterate through the neighboring patch positions and add them to the list
    for (int row = minRow; row <= maxRow; ++row) {
        for (int col = minCol; col <= maxCol; ++col) {
            if (row != x || col != y) {  // Exclude the current patch
                neighbours.push_back(&patches[row][col]);
            }
        }
    }
    neighbourLookup = neighbours;
}

void Patch::diffuseChemical() {
    float chemicalAverage = 0;
    chemicalAverage += neighbourLookup[0]->chemical;
    chemicalAverage += neighbourLookup[1]->chemical;
    chemicalAverage += neighbourLookup[2]->chemical;
    if (neighbourLookup.size() == 8) {
        chemicalAverage += neighbourLookup[3]->chemical;
        chemicalAverage += neighbourLookup[4]->chemical;
        chemicalAverage += neighbourLookup[5]->chemical;
        chemicalAverage += neighbourLookup[6]->chemical;
        chemicalAverage += neighbourLookup[7]->chemical;

    } else {
        for (int i = 3; i < neighbourLookup.size(); i++) {
            chemicalAverage += neighbourLookup[i]->chemical;
        }
    }
    if (chemicalAverage == 0.0) {
        return;
    }
    chemical = (float(1.0) - DECAY_FACTOR) * chemical + (TOTAL_DIFFUSION) * chemicalAverage / 8 * DECAY_FACTOR;
    if (chemical < 0.000001) {
        chemical = 0.0;
    }
    //chemical = chemicalAverage;

}