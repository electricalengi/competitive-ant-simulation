#ifndef COMPETITIVE_ANT_SIMULATION_PATCH_H
#define COMPETITIVE_ANT_SIMULATION_PATCH_H

#include <vector>
#include "Constants.h"

class Patch {
public:
    float chemical;
    int food;
    bool nest;
    float nestScent;
    int x{};
    int y{};
    std::vector<Patch*> neighbourLookup;

    explicit Patch(float c = 0, int f = 0, bool n = false, float s = 0);

    void getNeighbours(Patch patches[][display_size]);

    void diffuseChemical();
};

#endif //COMPETITIVE_ANT_SIMULATION_PATCH_H
