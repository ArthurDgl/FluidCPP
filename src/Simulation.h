//
// Created by arthu on 6/26/2024.
//

#ifndef SIMULATION_H
#define SIMULATION_H

#include <SFML/System/Vector2.hpp>


class Simulation {
public:
    float targetTime;
    float simulatedTime;
    float timeStep;

    int resolutionX;
    int resolutionY;

    float diffusionStrength;
    float diffusionSteps;

    bool** inert;
    bool** solid;

    float** densities;
    sf::Vector2f** velocities;

    Simulation(int resolutionX, int resolutionY, float diffusionStrength, float diffusionSteps, float timeStep);
    Simulation(int resolutionX, int resolutionY) : Simulation(resolutionX, resolutionY, 0.5, 10, 0.001) {};

    void tick(float dt);

    void diffuseDensity(float dt);
};



#endif //SIMULATION_H
