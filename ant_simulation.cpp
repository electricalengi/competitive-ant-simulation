#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>

const int maxTicks = 100;
const int populationSize = 50;
const int display_size = 512;

class Patch {
public:
	float chemical;
	int food;
	bool nest;
	float nestScent;
	int foodSourceNumber;

	Patch(int c = 0, int f = 0, bool n = false, int s = 0, int fsn = 0) : chemical(c), food(f), nest(n), nestScent(s), foodSourceNumber(fsn) {}
};

class Soldier {
public:
	int x, y;
	int colony;

	Soldier(int startX, int startY, bool hf, int fac) : x(startX), y(startY), colony(fac) {}
	
	void lookForEnemy(const std::vector<Patch>& patches) {
		// todo
	}
	
	void wiggle() {
		// todo
	}
};

class Worker {
public:
	int x, y;
	int colony;
	bool hasFood;
	float directionDegrees,


	Worker(int startX, int startY, int fac, bool hf) : x(startX), y(startY), colony(fac), hasFood(hf), directionDegrees(0.0) {}

	void lookForFood(const std::vector<Patch>& patches) {
		Patch& currentPatch = patches[x][y]; // Get the patch the ant is on

		if (currentPatch.food > 0) {
			hasFood = true;
			currentPatch.food = 0;
		}
		else {
			// No food on the patch, so the ant wiggles
			wiggle();

			// Move the ant one pixel in the direction they face
			float deltaX = cos(direction) * 1.0; // Move 1 pixel in x direction
			float deltaY = sin(direction) * 1.0; // Move 1 pixel in y direction

			// Update ant's position while ensuring it stays within the display boundary
			x = std::max(0, std::min(display_size - 1, x + static_cast<int>(deltaX)));
			y = std::max(0, std::min(display_size - 1, y + static_cast<int>(deltaY)));
		}
	}

	void returnToNest(const std::vector<Patch>& patches) {
		Patch& currentPatch = patches[x][y];
		if (currentPatch.nest) {
			hasFood = false;
		}
		else {

		}
	}

	void wiggle() {
		// Generate a random angle change between -20 and 20 degrees
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(-20.0, 20.0);
		float randomAngleChange = dis(gen);

		// Update the direction based on the random angle change
		updateDirection(randomAngleChange);
	}

	// Update direction based on a change in angle (degrees)
	void updateDirection(float deltaAngleDegrees) {
		directionDegrees += deltaAngleDegrees;
		// Ensure direction stays within 0 to 360 degrees
		if (directionDegrees >= 360.0) {
			directionDegrees -= 360.0;
		}
		else if (directionDegrees < 0.0) {
			directionDegrees += 360.0;
		}
	}
};



class Simulation {
public:
		Worker workers[populationSize];
	Patch patches[display_size][display_size];
	int tick = 0;


	int nestx = display_size / 2;
	int nesty = display_size / 2;
	int food_1_x = 100;
	int food_1_y = 100;
	

	Simulation() {
		initialiseWorkers();
		initialisePatches();
	}

	void initialisePatches() {
		for (int x = 0; x < display_size; x++) {
			for (int y = 0; y < display_size; y++) {
				Patch& patch = patches[x][y];
				float distance_nest = pow(nestx - x, 2) + pow(nesty - y, 2);
				float distance_food = pow(food_1_x - x, 2) + pow(food_1_y - y, 2);

				if (distance_food < 30) {
					patch.foodSourceNumber = 1;
					patch.food = 10;
				}

				if (distance_nest < 50) {
					patch.nest = true;
				}

				patch.nestScent = 1 / distance_nest;
			}
		}
	}

	void initialiseWorkers() {
		for (int index = 0; index < populationSize; index++) {
			Worker& worker = workers[index];
			worker.x = 0;
			worker.y = 0;
		}
	}

	void go() {
		if (tick < maxTicks) {
			for (int index = 0; index < populationSize; index++) {
				Worker& worker = workers[index];
				if (worker.hasFood) {
					worker.returnToNest(patches);
				}
				else { worker.lookForFood(patches); }
			}
			tick = tick + 1;
		}
	}
}