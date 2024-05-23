cmake_minimum_required(VERSION 3.26)
project(competitive_ant_simulation)

set(CMAKE_CXX_STANDARD 14)

add_executable(competitive_ant_simulation
        ant_simulation.cpp
        Patch.cpp
        Patch.h
        Constants.h
        Simulation.cpp
        Simulation.h
        Worker.cpp
        Worker.h
)