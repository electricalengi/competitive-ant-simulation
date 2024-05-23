#include "Worker.h"
#include "Patch.h"
#include <cmath>
#include <random>
#include <cstdlib>

static std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis;

int fitness = 0;

Worker::Worker(float cs, float ns,
               float c, int startX, int startY, bool hf, float d) : x(startX), y(startY), hasFood(hf), direction(d), chemical_sensitivity(cs), nest_sensitivity(ns), crazy(c), dis(-crazy, crazy) {
    std::uniform_real_distribution<float> initialDis(0, 360.0);
    direction = initialDis(gen);
}

void Worker::lookForFood(Patch patches[][DISPLAY_SIZE]) {
    Patch& currentPatch = patches[x][y];

    if (currentPatch.food > 0) {
        hasFood = true;
        currentPatch.food = 0;
        fitness += 1;
    }
    else {
        sniff(currentPatch, true);
    }
}

void Worker::returnToNest(Patch patches[][DISPLAY_SIZE], int &totalFood) {
    Patch& currentPatch = patches[x][y];
    currentPatch.chemical += 1;
    if (currentPatch.nest) {
        hasFood = false;
        totalFood++;
        fitness += 10;
    }
    else {
        sniff(currentPatch, false);
    }
}

void Worker::sniff(Patch& currentPatch, bool lookingForFood) {
    Patch* maxScentPatch = nullptr;
    if (!lookingForFood) {
        std::vector<Patch*> neighbours = currentPatch.neighbourLookup;
        float maxScent = 0;
        for (auto currentNeighbour : neighbours) {
            if (currentNeighbour->nestScent > maxScent) {
                maxScentPatch = currentNeighbour;
                maxScent = currentNeighbour->nestScent;
            }
        }
    } else {
        std::vector<Patch*> neighbours = currentPatch.neighbourLookup;
        float maxScent = 0;
        float scent;
        for (auto currentNeighbour : neighbours) {
            scent = currentNeighbour->chemical * chemical_sensitivity + currentNeighbour->nestScent * nest_sensitivity;
            if (scent > maxScent) {
                maxScentPatch = currentNeighbour;
                maxScent = scent;
            }
        }
    }

    if (maxScentPatch == nullptr) {
        wiggle();
    }
    else {
        // Calculate the angle towards the patch with max scent
        int deltaX = maxScentPatch->x - x;
        int deltaY = maxScentPatch->y - y;
        float targetAngle = std::atan2(deltaY, deltaX) * 180.0 / PI;
        // Calculate the angle difference between the target angle and current direction
        float angleDifference = targetAngle - direction;

        // Update the direction based on the angle difference
        updateDirection(angleDifference);
        wiggle();
    }

    // Move the ant one pixel in the direction they face
    float moveX = cos(direction * PI / 180.0) * 1.0;
    float moveY = sin(direction * PI / 180.0) * 1.0;

    // Update ant's position while ensuring it stays within the display boundary
    int newX = x + static_cast<int>(std::round(moveX));
    int newY = y + static_cast<int>(std::round(moveY));
    // Check if the new position would take the ant off the map
    if (newX < 0 || newX >= DISPLAY_SIZE || newY < 0 || newY >= DISPLAY_SIZE) {
        updateDirection(180);
    } else {
        // Update ant's position
        x = newX;
        y = newY;
    }

}



void Worker::wiggle() {
    // Generate a random angle change between -20 and 20 degrees

    float randomAngleChange = dis(gen);

    // Update the direction based on the random angle change
    updateDirection(randomAngleChange);
}

// Update direction based on a change in angle (degrees)
void Worker::updateDirection(float deltaAngleDegrees) {
    direction += deltaAngleDegrees;
    // Ensure direction stays within 0 to 360 degrees
    if (direction >= 360.0) {
        direction -= 360.0;
    }
    else if (direction < 0.0) {
        direction += 360.0;
    }
}

int getFitness() {
    return fitness;
}