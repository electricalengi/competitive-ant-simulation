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
	float direction,


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
			std::vector<Patch*> neighbours = getNeighbours(patches);
			Patch* maxScentPatch = nullptr;
			float maxScent = 0;
			for (int neighbourIndex; neighbourIndex < neighbours.size(); neighbourIndex++) {
				Patch* currentNeighbour = neighbours[neighbourIndex];
				if (currentNeighbour->nestScent > maxScent) {
					maxScentPatch = currentNeighbour;
					maxScent = currentNeighbour->nestScent;
				}	
			}
			if (maxScentPatch == nullptr) {
				wiggle();
				return;
			}
			else {
				// Calculate the angle towards the patch with max scent
				float deltaX = maxScentPatch->x - x;
				float deltaY = maxScentPatch->y - y;
				float targetAngle = std::atan2(deltaY, deltaX) * 180.0 / M_PI;

				// Calculate the angle difference between the target angle and current direction
				float angleDifference = targetAngle - directionDegrees;

				// Update the direction based on the angle difference
				updateDirection(angleDifference);

				// Move the ant one pixel in the direction they face
				float moveX = cos(directionDegrees * M_PI / 180.0) * 1.0;
				float moveY = sin(directionDegrees * M_PI / 180.0) * 1.0;

				// Update ant's position while ensuring it stays within the display boundary
				x = std::max(0, std::min(display_size - 1, static_cast<int>(x + moveX)));
				y = std::max(0, std::min(display_size - 1, static_cast<int>(y + moveY)));

			}
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
		direction += deltaAngleDegrees;
		// Ensure direction stays within 0 to 360 degrees
		if (direction >= 360.0) {
			direction -= 360.0;
		}
		else if (direction < 0.0) {
			direction += 360.0;
		}
	}

	std::vector<Patch*> getNeighbours(std::vector<std::vector<Patch>>& patches) {
		std::vector<Patch*> neighbours;

		// Define the range of neighboring patches
		int minRow = std::max(0, x - 1);
		int maxRow = std::min(display_size - 1, x + 1);
		int minCol = std::max(0, y - 1);
		int maxCol = std::min(display_size - 1, y + 1);

		// Iterate through the neighboring patch positions and add them to the list
		for (int row = minRow; row <= maxRow; ++row) {
			for (int col = minCol; col <= maxCol; ++col) {
				if (row != x || col != y) {  // Exclude the current patch
					neighbours.push_back(&patches[row][col]);
				}
			}
		}

		return neighbours;
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
			// Print worker's state and location
			std::cout << "Tick " << tick << ", Worker " << index
				<< " - Location: (" << worker.x << ", " << worker.y << ")"
				<< ", Has Food: " << (worker.hasFood ? "Yes" : "No") << std::endl;
			tick = tick + 1;
		}
	}
}


int main() {
	Simulation sim;
	sim.go();
}