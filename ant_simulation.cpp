#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <fstream>

const int maxTicks = 500;
const int populationSize = 300;
const int display_size = 100;
const float totalDiffusion = 0.8;
const float decayFactor = 0.5 ;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(-45.0, 45.0);

using namespace std;
	
class Patch {
public:
	float chemical;
	int food;
	bool nest;
	float nestScent;
	int foodSourceNumber;
    int x{};
    int y{};
    std::vector<Patch *> neighbourLookup;

	explicit Patch(float c = 0, int f = 0, bool n = false, float s = 0, int fsn = 0) : chemical(c), food(f), nest(n), nestScent(s), foodSourceNumber(fsn) {
    }


    void getNeighbours(Patch patches[][display_size]) {
        std::vector<Patch *> neighbours;

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
        neighbourLookup = neighbours;
    }

    void diffuseChemical(Patch patches[display_size][display_size]) {
        float chemicalAverage = 0;
        for (auto currentNeighbour : neighbourLookup) {
            chemicalAverage += currentNeighbour->chemical;
        }
        chemical = (float(1.0) - decayFactor) * chemical + (totalDiffusion) * chemicalAverage / 8 * decayFactor;
        if (chemical < 0.000001) {
            chemical = 0.0;
        }
        //chemical = chemicalAverage;

    }
};

class Worker {
public:
	int x, y;
	bool hasFood;
	float direction;


	explicit Worker(int startX = display_size/2, int startY = display_size/2, int fac = 0, bool hf = false, float d = 270.0) : x(startX), y(startY), hasFood(hf), direction(d) {}

	void lookForFood(Patch patches[][display_size]) {
		Patch& currentPatch = patches[x][y];

		if (currentPatch.food > 0) {
			hasFood = true;
			currentPatch.food = 0;
		}
		else {
            sniff(currentPatch, patches, true);
		}
	}

	void returnToNest(Patch patches[][display_size]) {
		Patch& currentPatch = patches[x][y];
        currentPatch.chemical += 1;
		if (currentPatch.nest) {
			hasFood = false;
		}
		else {
            sniff(currentPatch, patches, false);
		}
	}

    void sniff(Patch& currentPatch, Patch patches[][display_size], bool lookingForFood) {
        float Patch::*targetAttribute;
        if (lookingForFood) {
            targetAttribute = &Patch::chemical;
        } else {
            targetAttribute = &Patch::nestScent;
        }

        std::vector<Patch*> neighbours = currentPatch.neighbourLookup;
        Patch* maxScentPatch = nullptr;
        float maxScent = 0;
        for (auto currentNeighbour : neighbours) {
            if (currentNeighbour->*targetAttribute > maxScent) {
                maxScentPatch = currentNeighbour;
                maxScent = currentNeighbour->*targetAttribute;
            }
        }
        if (maxScentPatch == nullptr) {
            wiggle();

        }
        else {
            // Calculate the angle towards the patch with max scent
            float deltaX = maxScentPatch->x - x;
            float deltaY = maxScentPatch->y - y;
            float targetAngle = std::atan2(deltaY, deltaX) * 180.0 / M_PI;
            // Calculate the angle difference between the target angle and current direction
            float angleDifference = targetAngle - direction;

            // Update the direction based on the angle difference
            updateDirection(angleDifference);
            wiggle();
        }

        // Move the ant one pixel in the direction they face
        float moveX = cos(direction * M_PI / 180.0) * 1.0;
        float moveY = sin(direction * M_PI / 180.0) * 1.0;

        // Update ant's position while ensuring it stays within the display boundary
        int newX = x + static_cast<int>(std::round(moveX));
        int newY = y + static_cast<int>(std::round(moveY));
        // Check if the new position would take the ant off the map
        if (newX < 0 || newX >= display_size || newY < 0 || newY >= display_size) {
            updateDirection(180);
        } else {
            // Update ant's position
            x = newX;
            y = newY;
        }

    }



	void wiggle() {
		// Generate a random angle change between -20 and 20 degrees

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
};



class Simulation {
public:
    Worker workers[populationSize];
	Patch patches[display_size][display_size];
	int tick = 0;


	int nest_x = display_size / 2;
	int nest_y = display_size / 2;
	int food_1_x = 75;
	int food_1_y = 75;
    int food_2_x = 50;
    int food_2_y = 25;


	Simulation() {
		//initialiseWorkers();
		initialisePatches();
	}

	void initialisePatches() {
		for (int x = 0; x < display_size; x++) {
			for (int y = 0; y < display_size; y++) {
				Patch& patch = patches[x][y];
				float distance_nest = pow(nest_x - x, 2) + pow(nest_y - y, 2);
				float distance_food1 = pow(food_1_x - x, 2) + pow(food_1_y - y, 2);
                float distance_food2 = pow(food_2_x - x, 2) + pow(food_2_y - y, 2);

				if (distance_food1 < 30) {
					patch.foodSourceNumber = 1;
					patch.food = 10;
				}

                if (distance_food2 < 20) {
                    patch.foodSourceNumber = 2;
                    patch.food = 10;
                }

				if (distance_nest < 50) {
					patch.nest = true;
				}
                patch.x = x;
                patch.y = y;
				patch.nestScent = 1 / distance_nest;
                patch.chemical = 0;
			}
		}
        for (auto & patche : patches) {
            for (auto & j : patche) {
                j.getNeighbours(patches);
            }
        }
	}

	void test() {
		while (tick < maxTicks) {
            printSimulationState(patches, workers);
            for (int i = 0; i < display_size; ++i) {
                for (int j = 0; j < display_size; ++j) {
                    patches[i][j].diffuseChemical(patches);
                }
            }

			for (auto & worker : workers) {
                if (worker.hasFood) {
					worker.returnToNest(patches);
				}
                else {
                    worker.lookForFood(patches);
                }
			}
            tick++;
		}
	}


    static void printSimulationState(const Patch displayPatches[][display_size], const Worker displayWorkers[populationSize]) {
        // Clear the screen (for visualization purposes)
        system("cls");

        for (int y = 0; y < display_size; ++y) {
            for (int x = 0; x < display_size; ++x) {
                bool printed = false;

                // Check if any worker is at this position
                for (int workerIndex = 0; workerIndex < populationSize; workerIndex ++) {
                    Worker worker = displayWorkers[workerIndex];
                    if (worker.x == x && worker.y == y) {
                        if (worker.hasFood) {
                            cout << 'F'; // Worker with food
                        } else {
                            cout << 'W'; // Worker without food
                        }

                        printed = true;
                        break;
                    }
                }

                if (!printed) {
                    // Check if any patch is at this position
                    if (displayPatches[x][y].nest) {
                        cout << 'N'; // Nest patch
                    } else if (displayPatches[x][y].food > 0) {
                        cout << 'P'; // Food patch
                    } else if ((displayPatches[x][y].chemical > 0)) {
                        cout << 'C';
                    } else  {
                        cout << ' ';
                    }
                }
            }
            cout << endl;
        }
    }

    void go(const std::string& filename) {
        std::vector<std::vector<char>> gridData(maxTicks, std::vector<char>(display_size * display_size));

        for (int tick = 0; tick < maxTicks; ++tick) {
            int j;
            for (int i = 0; i < display_size; ++i) {
                j = 0;
                for (; j < display_size - 9; j += 10) {
                    patches[i][j].diffuseChemical(patches);
                    patches[i][j + 1].diffuseChemical(patches);
                    patches[i][j + 2].diffuseChemical(patches);
                    patches[i][j + 3].diffuseChemical(patches);
                    patches[i][j + 4].diffuseChemical(patches);
                    patches[i][j + 5].diffuseChemical(patches);
                    patches[i][j + 6].diffuseChemical(patches);
                    patches[i][j + 7].diffuseChemical(patches);
                    patches[i][j + 8].diffuseChemical(patches);
                    patches[i][j + 9].diffuseChemical(patches);
                }

                // Handle the remaining elements (less than 10) if any
                for (; j < display_size; ++j) {
                    patches[i][j].diffuseChemical(patches);
                }
            }


            for (auto &worker: workers) {
                if (worker.hasFood) {
                    worker.returnToNest(patches);
                } else {
                    worker.lookForFood(patches);
                }
            }

//          Write the state of each cell in the grid
            for (int x = 0; x < display_size; ++x) {
                for (int y = 0; y < display_size; ++y) {
                    bool printed = false;

                    // Check if any worker is at this position
                    for (int workerIndex = 0; workerIndex < populationSize; workerIndex++) {
                        Worker worker = workers[workerIndex];
                        if (worker.x == x && worker.y == y) {
                            if (worker.hasFood) {
                                gridData[tick][x * display_size + y] = '4'; // Worker with food
                            } else {
                                gridData[tick][x * display_size + y] = '5'; // Worker without food
                            }
                            printed = true;
                            break;
                        }
                    }

                    if (!printed) {
                        // Check if any patch is at this position
                        if (patches[x][y].nest) {
                            gridData[tick][x * display_size + y] = '1'; // Nest patch
                        } else if (patches[x][y].food > 0) {
                            gridData[tick][x * display_size + y] = '2'; // Food patch
                        } else if ((patches[x][y].chemical > 0)) {
                            gridData[tick][x * display_size + y] = '3'; // Chemical patch
                        } else {
                            gridData[tick][x * display_size + y] = '0';
                        }
                    }

                }
            }
        }


        std::ofstream outputFile(filename);

        if (!outputFile.is_open()) {
            std::cerr << "Error opening file for writing: " << filename << std::endl;
            return;
        }

        for (int tick = 0; tick < maxTicks; ++tick) {
            // Write the header row for the CSV file (column names)
            if (tick == 0) {
                outputFile << "Tick,";
                for (int x = 0; x < display_size; ++x) {
                    for (int y = 0; y < display_size; ++y) {
                        outputFile << "X" << x << "Y" << y << ",";
                    }
                }
                outputFile << std::endl;
            }

            // Write the current tick number
            outputFile << tick << ",";

            for (int x = 0; x < display_size; ++x) {
                for (int y = 0; y < display_size; ++y) {
                    outputFile << gridData[tick][x * display_size + y] << ',';
                }
            }
            outputFile << std::endl;
        }
        outputFile.close();
    }

};


int main() {
	Simulation sim;
    sim.go("result.csv");
	//sim.test();
}