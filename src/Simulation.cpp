//
// Created by arthu on 6/26/2024.
//

#include "Simulation.h"

Simulation::Simulation(int resolutionX, int resolutionY, float diffusionStrength, float diffusionSteps) {
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
    diffuseDensity();
}

void Simulation::diffuseDensity() {
    for (int i = 0; i < diffusionSteps; i++) {
        float temporary[resolutionX][resolutionY];

        for (int x = 0; x < resolutionX; x++) {
            for (int y = 0; y < resolutionY; y++) {
                if (inert[x][y]) continue;

                temporary[x][y] = (densities[x+1][y] + densities[x][y+1] + densities[x-1][y] + densities[x][y-1]) / 4;
            }
        }

        for (int x = 0; x < resolutionX; x++) {
            for (int y = 0; y < resolutionY; y++) {
                if (inert[x][y]) continue;
                densities[x][y] = temporary[x][y];
            }
        }
    }
}