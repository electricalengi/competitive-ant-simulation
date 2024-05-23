#include "Simulation.h"
#include <fstream>
#include <vector>
#include <cstdlib>

Simulation::Simulation() {
    initialisePatches();
}

void Simulation::initialisePatches() {
    for (int x = 0; x < DISPLAY_SIZE; x++) {
        for (int y = 0; y < DISPLAY_SIZE; y++) {
            Patch& patch = patches[x][y];
            int distance_nest = (nest_x - x) * (nest_x - x) + (nest_y - y) * (nest_y - y);
            int distance_food1 = (food_1_x - x) * (food_1_x - x) + (food_1_y - y) * (food_1_y - y);
            int distance_food2 = (food_2_x - x) * (food_2_x - x) + (food_2_y - y) * (food_2_y - y);

            if (distance_food1 < 30) {
                patch.food = 1;
            }

            if (distance_food2 < 20) {
                patch.food = 1;
            }

            if (distance_nest < 50) {
                patch.nest = true;
            }
            patch.x = x;
            patch.y = y;
            patch.nestScent = 1.0 / distance_nest;
            patch.chemical = 0;
        }
    }
    for (auto & patch : patches) {
        for (auto & j : patch) {
            j.getNeighbours(patches);
        }
    }
}

void Simulation::go(const std::string& filename) {
    int totalFood = 0;
    std::vector<int> totalFoods(MAX_TICKS);
    std::vector<std::vector<char>> gridData(MAX_TICKS, std::vector<char>(DISPLAY_SIZE * DISPLAY_SIZE));

    for (int tick = 0; tick < MAX_TICKS; ++tick) {
        int j;
        for (auto & patch : patches) {
            for (auto & j : patch) {
                j.diffuseChemical();
            }
        }


        for (auto &worker: workers) {
            if (worker.hasFood) {
                worker.returnToNest(patches, totalFood);
            } else {
                worker.lookForFood(patches);
            }
        }

        totalFoods[tick] = totalFood;

        // Write the state of each cell in the grid
        for (int x = 0; x < DISPLAY_SIZE; ++x) {
            for (int y = 0; y < DISPLAY_SIZE; ++y) {
                bool printed = false;

                // Check if any worker is at this position
                for (auto worker : workers) {
                    if (worker.x == x && worker.y == y) {
                        if (worker.hasFood) {
                            gridData[tick][x * DISPLAY_SIZE + y] = WORKER_WITH_FOOD; // Worker with food
                        } else {
                            gridData[tick][x * DISPLAY_SIZE + y] = WORKER_WITHOUT_FOOD; // Worker without food
                        }
                        printed = true;
                        break;
                    }
                }

                if (!printed) {
                    // Check if any patch is at this position
                    if (patches[x][y].nest) {
                        gridData[tick][x * DISPLAY_SIZE + y] = NEST; // Nest patch
                    } else if (patches[x][y].food > 0) {
                        gridData[tick][x * DISPLAY_SIZE + y] = FOOD; // Food patch
                    } else if ((patches[x][y].chemical > 0)) {
                        gridData[tick][x * DISPLAY_SIZE + y] = CHEMICAL; // Chemical patch
                    } else {
                        gridData[tick][x * DISPLAY_SIZE + y] = EMPTY;
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

    for (int tickPrint = 0; tickPrint < MAX_TICKS; ++tickPrint) {
        // Write the header row for the CSV file (column names)
        if (tickPrint == 0) {
            outputFile << "Tick,";
            for (int x = 0; x < DISPLAY_SIZE; ++x) {
                for (int y = 0; y < DISPLAY_SIZE; ++y) {
                    outputFile << "X" << x << "Y" << y << ",";
                }
            }
            outputFile << "Total Food";
            outputFile << std::endl;
        }

        // Write the current tickPrint number
        outputFile << tickPrint << ",";

        for (int x = 0; x < DISPLAY_SIZE; ++x) {
            for (int y = 0; y < DISPLAY_SIZE; ++y) {
                outputFile << gridData[tickPrint][x * DISPLAY_SIZE + y] << ',';
            }
        }
        outputFile << totalFoods[tickPrint];

        outputFile << std::endl;
    }
    outputFile.close();

    // Form the command to execute the Python script
    std::string command = "python C:/Users/chigg/CLionProjects/competitive-ant-simulation/cmake-build-debug/game.py";

    // Execute the command
    int status = std::system(command.c_str());

    // Check if the command executed successfully
    if (status != 0) {
        std::cerr << "Error executing the Python script." << std::endl;
    }
}
