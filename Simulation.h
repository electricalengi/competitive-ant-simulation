#ifndef COMPETITIVE_ANT_SIMULATION_SIMULATION_H
#define COMPETITIVE_ANT_SIMULATION_SIMULATION_H

#include <iostream>
#include "Patch.h"
#include "Worker.h"

class Simulation {
public:
    Worker workers[populationSize];
    Patch patches[display_size][display_size];

    int nest_x = display_size / 2;
    int nest_y = display_size / 2;
    int food_1_x = 75;
    int food_1_y = 75;
    int food_2_x = 50;
    int food_2_y = 25;

    Simulation();

    void initialisePatches();

    void go(const std::string& filename);
};
#endif //COMPETITIVE_ANT_SIMULATION_SIMULATION_H
