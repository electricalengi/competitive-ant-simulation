#ifndef COMPETITIVE_ANT_SIMULATION_WORKER_H
#define COMPETITIVE_ANT_SIMULATION_WORKER_H

#include "Constants.h"
#include "Patch.h"
#include <random>
#include <cmath>

class Worker {
public:
    int x, y;
    bool hasFood;
    float direction;

    explicit Worker(int startX = display_size / 2, int startY = display_size / 2, bool hf = false, float d = 270.0);

    void lookForFood(Patch patches[][display_size]);

    void returnToNest(Patch patches[][display_size]);

    void sniff(Patch& currentPatch, bool lookingForFood);

    void wiggle();

    void updateDirection(float deltaAngleDegrees);
};

#endif //COMPETITIVE_ANT_SIMULATION_WORKER_H
