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

    explicit Worker(int startX = DISPLAY_SIZE / 2, int startY = DISPLAY_SIZE / 2, bool hf = false, float d = 0);

    void lookForFood(Patch patches[][DISPLAY_SIZE]);

    void returnToNest(Patch patches[][DISPLAY_SIZE], int &);

    void sniff(Patch& currentPatch, bool lookingForFood);

    void wiggle();

    void updateDirection(float deltaAngleDegrees);
};

#endif //COMPETITIVE_ANT_SIMULATION_WORKER_H
