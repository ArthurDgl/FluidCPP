//
// Created by arthu on 6/26/2024.
//

#ifndef SIMULATION_H
#define SIMULATION_H

#include <SFML/System/Vector2.hpp>
#include <cmath>


class Simulation {
public:
    float targetTime;
    float simulatedTime;
    float timeStep;

    int resolutionX;
    int resolutionY;

    float diffusionStrength;
    int diffusionSteps;

    int divergenceSteps;

    bool** inert;
    bool** solid;

    float** densities;
    sf::Vector2f** velocities;

    float inflowSpeed;

    Simulation(int resolutionX, int resolutionY, float diffusionStrength, int diffusionSteps, float timeStep, int divergenceSteps, float inflowSpeed);
    Simulation(int resolutionX, int resolutionY) : Simulation(resolutionX, resolutionY, 0.05, 10, 0.01, 50, 20) {};

    void tick(float dt);

    void diffuseDensity(float dt);
    void diffuseVelocity(float dt);

    float sampleDensity(float xPos, float yPos);
    sf::Vector2f sampleVelocity(float xPos, float yPos);

    float outOfBoundsDensity(float xPos, float yPos);
    sf::Vector2f outOfBoundsVelocity(float xPos, float yPos);

    void advectDensity(float dt);
    void advectVelocity(float dt);

    bool isOutOfBounds(float xPos, float yPos);

    void clearDivergence();
    float** computeDivergence();

    void setInflow();

    void setSolidBlock(int x1, int y1, int x2, int y2);

    void setSolidTriangle(int x1, int y1, int l1, int h, int l2);

    void setSolidCircle(int x, int y, int radius);
};



#endif //SIMULATION_H
