//
// Created by arthu on 6/26/2024.
//

#include "Simulation.h"

Simulation::Simulation(int resolutionX, int resolutionY, float diffusionStrength, float diffusionSteps, float timeStep) {
    this->targetTime = 0;
    this->simulatedTime = 0;
    this->timeStep = timeStep;

    this->resolutionX = resolutionX;
    this->resolutionY = resolutionY;

    this->diffusionStrength = diffusionStrength;
    this->diffusionSteps = diffusionSteps;

    inert = new bool*[resolutionX];
    solid = new bool*[resolutionX];
    densities = new float*[resolutionX];
    velocities = new sf::Vector2f*[resolutionX];

    for (int x = 0; x < resolutionX; x++) {
        inert[x] = new bool[resolutionY];
        solid[x] = new bool[resolutionY];
        densities[x] = new float[resolutionY];
        velocities[x] = new sf::Vector2<float>[resolutionY];

        for (int y = 0; y < resolutionY; y++) {
            inert[x][y] = false;
            solid[x][y] = false;

            densities[x][y] = 0.0f;
            velocities[x][y] = sf::Vector2f(0, 0);

            // Bounds are always inert
            if (x == 0 || y == 0 || x == resolutionX - 1 || y == resolutionY - 1) inert[x][y] = true;

            if (x > resolutionX*2.0/5.0 && x < resolutionX*3.0/5.0 && y > resolutionY*2.0/5.0 && y < resolutionY*3.0/5.0) {
                densities[x][y] = 1;
            }
        }
    }
}

void Simulation::tick(float dt) {
    targetTime += dt;
    while (targetTime > simulatedTime) {
        simulatedTime += timeStep;
        diffuseDensity(timeStep);
    }
}

void Simulation::diffuseDensity(float dt) {
    float solution[resolutionX][resolutionY];
    float mult = dt*diffusionStrength;
    float denom = 1.0f / (1.0f + 4.0f*mult);

    for (int i = 0; i < diffusionSteps; i++) {
        for (int x = 0; x < resolutionX; x++) {
            for (int y = 0; y < resolutionY; y++) {
                if (i == 0) {
                    solution[x][y] = densities[x][y];
                    continue;
                }
                if (inert[x][y]) continue;
                float sum = solution[x+1][y] + solution[x][y+1] + solution[x-1][y] + solution[x][y-1];
                densities[x][y] = (densities[x][y] + mult*sum) * denom;
            }
        }
    }
}