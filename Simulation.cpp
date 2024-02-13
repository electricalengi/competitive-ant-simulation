#include "Simulation.h"
#include <fstream>
#include <vector>

Simulation::Simulation() {
    initialisePatches();
}

void Simulation::initialisePatches() {
    for (int x = 0; x < display_size; x++) {
        for (int y = 0; y < display_size; y++) {
            Patch& patch = patches[x][y];
            int distance_nest = (nest_x - x) * (nest_x - x) + (nest_y - y) * (nest_y - y);
            int distance_food1 = (food_1_x - x) * (food_1_x - x) + (food_1_y - y) * (food_1_y - y);
            int distance_food2 = (food_2_x - x) * (food_2_x - x) + (food_2_y - y) * (food_2_y - y);

            if (distance_food1 < 30) {
                patch.food = 10;
            }

            if (distance_food2 < 20) {
                patch.food = 10;
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
    std::vector<std::vector<char>> gridData(maxTicks, std::vector<char>(display_size * display_size));

    for (int tick = 0; tick < maxTicks; ++tick) {
        int j;
        for (auto & patch : patches) {
            j = 0;
            for (; j < display_size - 9; j += 10) {
                patch[j].diffuseChemical();
                patch[j + 1].diffuseChemical();
                patch[j + 2].diffuseChemical();
                patch[j + 3].diffuseChemical();
                patch[j + 4].diffuseChemical();
                patch[j + 5].diffuseChemical();
                patch[j + 6].diffuseChemical();
                patch[j + 7].diffuseChemical();
                patch[j + 8].diffuseChemical();
                patch[j + 9].diffuseChemical();
            }

            // Handle the remaining elements (less than 10) if any
            for (; j < display_size; ++j) {
                patch[j].diffuseChemical();
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
                for (auto worker : workers) {
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

    for (int tickPrint = 0; tickPrint < maxTicks; ++tickPrint) {
        // Write the header row for the CSV file (column names)
        if (tickPrint == 0) {
            outputFile << "Tick,";
            for (int x = 0; x < display_size; ++x) {
                for (int y = 0; y < display_size; ++y) {
                    outputFile << "X" << x << "Y" << y << ",";
                }
            }
            outputFile << std::endl;
        }

        // Write the current tickPrint number
        outputFile << tickPrint << ",";

        for (int x = 0; x < display_size; ++x) {
            for (int y = 0; y < display_size; ++y) {
                outputFile << gridData[tickPrint][x * display_size + y] << ',';
            }
        }
        outputFile << std::endl;
    }
    outputFile.close();
}
