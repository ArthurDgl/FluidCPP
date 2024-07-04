//
// Created by arthu on 6/26/2024.
//

#include "simulation.h"


Simulation::Simulation(int resolutionX, int resolutionY, float diffusionStrength, int diffusionSteps, float timeStep, int divergenceSteps, float inflowSpeed) {
    this->targetTime = 0;
    this->simulatedTime = 0;
    this->timeStep = timeStep;

    this->resolutionX = resolutionX;
    this->resolutionY = resolutionY;

    this->diffusionStrength = diffusionStrength;
    this->diffusionSteps = diffusionSteps;

    this->divergenceSteps = divergenceSteps;

    inert = new bool*[resolutionX];
    solid = new bool*[resolutionX];
    densities = new float*[resolutionX];
    velocities = new sf::Vector2f*[resolutionX];

    this->inflowSpeed = inflowSpeed;

    for (int x = 0; x < resolutionX; x++) {
        inert[x] = new bool[resolutionY];
        solid[x] = new bool[resolutionY];
        densities[x] = new float[resolutionY];
        velocities[x] = new sf::Vector2f[resolutionY];

        for (int y = 0; y < resolutionY; y++) {
            inert[x][y] = false;
            solid[x][y] = false;

            densities[x][y] = 0.0f;
            velocities[x][y] = sf::Vector2f(0, 0);

            // Bounds are always inert
            if (x == 0 || y == 0 || x == resolutionX - 1 || y == resolutionY - 1) inert[x][y] = true;

            // Upper and Lower walls of wind tunnel
            if (y == 0 ||y == resolutionY - 1) solid[x][y] = true;

            // if (x > resolutionX*2.0/5.0 && x < resolutionX*3.0/5.0 && y > resolutionY*2.0/5.0 && y < resolutionY*3.0/5.0) {
            //     densities[x][y] = 1;
            // }
        }
    }
}

void Simulation::tick(float dt) {
    targetTime += dt;

    if (targetTime - simulatedTime > 0.10f) {
        simulatedTime = targetTime;
        return;
    }

    while (targetTime > simulatedTime) {
        simulatedTime += timeStep;

        setInflow();

        diffuseDensity(timeStep);
        diffuseVelocity(timeStep);

        advectDensity(timeStep);
        advectVelocity(timeStep);

        clearDivergence();
    }
}

void Simulation::diffuseDensity(float dt) {
    float** solution = new float*[resolutionX];
    for (int x = 0; x < resolutionX; x++) {
        solution[x] = new float[resolutionY];
    }

    float mult = dt*diffusionStrength;
    float denom = 1.0f / (1.0f + 4.0f*mult);

    for (int i = 0; i < diffusionSteps + 1; i++) {
        for (int x = 0; x < resolutionX; x++) {
            for (int y = 0; y < resolutionY; y++) {
                if (i == 0) {
                    solution[x][y] = densities[x][y];
                    continue;
                }
                if (inert[x][y]) continue;
                float sum = solution[x+1][y] + solution[x][y+1] + solution[x-1][y] + solution[x][y-1];
                solution[x][y] = (densities[x][y] + mult*sum) * denom;

                if (i == diffusionSteps) {
                    densities[x][y] = solution[x][y];
                }
            }
        }
    }

    for (int x = 0; x < resolutionX; x++) {
        delete[] solution[x];
    }
    delete[] solution;
}

void Simulation::diffuseVelocity(float dt) {
    sf::Vector2f** solution = new sf::Vector2f*[resolutionX];
    for (int x = 0; x < resolutionX; x++) {
        solution[x] = new sf::Vector2f[resolutionY];
    }

    float mult = dt*diffusionStrength;
    float denom = 1.0f / (1.0f + 4.0f*mult);

    for (int i = 0; i < diffusionSteps + 1; i++) {
        for (int x = 0; x < resolutionX; x++) {
            for (int y = 0; y < resolutionY; y++) {
                if (i == 0) {
                    solution[x][y] = velocities[x][y];
                    continue;
                }
                if (inert[x][y]) continue;
                sf::Vector2f sum = solution[x+1][y] + solution[x][y+1] + solution[x-1][y] + solution[x][y-1];
                solution[x][y] = (velocities[x][y] + mult*sum) * denom;

                if (i == diffusionSteps) {
                    velocities[x][y] = solution[x][y];
                }
            }
        }
    }

    for (int x = 0; x < resolutionX; x++) {
        delete[] solution[x];
    }
    delete[] solution;
}

float Simulation::sampleDensity(float xPos, float yPos) {
    if (isOutOfBounds(xPos, yPos)) return outOfBoundsDensity(xPos, yPos);

    double xD;
    double yD;
    const float fx = modf(xPos, &xD);
    const float fy = modf(yPos, &yD);

    const int x = static_cast<int>(xD);
    const int y = static_cast<int>(yD);

    const float v1 = (1 - fx) * densities[x][y] + fx * densities[x+1][y];
    const float v2 = (1 - fx) * densities[x][y+1] + fx * densities[x+1][y+1];

    return (1 - fy) * v1 + fy * v2;
}

sf::Vector2f Simulation::sampleVelocity(float xPos, float yPos) {
    if (isOutOfBounds(xPos, yPos)) return outOfBoundsVelocity(xPos, yPos);

    double xD;
    double yD;
    const float fx = modf(xPos, &xD);
    const float fy = modf(yPos, &yD);

    const int x = static_cast<int>(xD);
    const int y = static_cast<int>(yD);

    const sf::Vector2f v1 = (1 - fx) * velocities[x][y] + fx * velocities[x+1][y];
    const sf::Vector2f v2 = (1 - fx) * velocities[x][y+1] + fx * velocities[x+1][y+1];

    return (1 - fy) * v1 + fy * v2;
}

float Simulation::outOfBoundsDensity(float xPos, float yPos) {
    if (xPos < 1) return 1.0f;
    return 0.0f;
}

sf::Vector2f Simulation::outOfBoundsVelocity(float xPos, float yPos) {
    if (xPos < 1) return sf::Vector2f(inflowSpeed, 0.0f);
    return sf::Vector2f(0.0f, 0.0f);
}

void Simulation::advectDensity(float dt) {
    float** temporary = new float*[resolutionX];

    for (int x = 0; x < resolutionX; x++) {
        temporary[x] = new float[resolutionY];
        for (int y = 0; y < resolutionY; y++) {
            const sf::Vector2f dVel = velocities[x][y] * dt;
            temporary[x][y] = sampleDensity(x - dVel.x, y - dVel.y);
        }
    }

    for (int x = 0; x < resolutionX; x++) {
        for (int y = 0; y < resolutionY; y++) {
            densities[x][y] = temporary[x][y];
        }
        delete[] temporary[x];
    }
    delete[] temporary;
}

void Simulation::advectVelocity(float dt) {
    sf::Vector2f** temporary = new sf::Vector2f*[resolutionX];

    for (int x = 0; x < resolutionX; x++) {
        temporary[x] = new sf::Vector2f[resolutionY];
        for (int y = 0; y < resolutionY; y++) {
            const sf::Vector2f dVel = velocities[x][y] * dt;
            temporary[x][y] = sampleVelocity(x - dVel.x, y - dVel.y);
        }
    }

    for (int x = 0; x < resolutionX; x++) {
        for (int y = 0; y < resolutionY; y++) {
            velocities[x][y] = temporary[x][y];
        }
        delete[] temporary[x];
    }
    delete[] temporary;
}

bool Simulation::isOutOfBounds(float xPos, float yPos) {
    return xPos < 0 || yPos < 0 || xPos >= resolutionX - 1 ||yPos >= resolutionY - 1;
}

void Simulation::clearDivergence() {
    float** divergence = computeDivergence();

    float** p = new float*[resolutionX];
    for (int x = 0; x < resolutionX; x++) {
        p[x] = new float[resolutionY];
        for (int y = 0; y < resolutionY; y++) {
            p[x][y] = 0;
        }
    }

    for (int i = 0; i < divergenceSteps; i++) {
        for (int x = 0; x < resolutionX; x++) {
            for (int y = 0; y < resolutionY; y++) {
                if (inert[x][y]) continue;

                float sumP = 0;
                int count = 0;

                if (!solid[x+1][y]) { sumP += p[x+1][y]; count++; }
                if (!solid[x-1][y]) { sumP += p[x-1][y]; count++; }
                if (!solid[x][y+1]) { sumP += p[x][y+1]; count++; }
                if (!solid[x][y-1]) { sumP += p[x][y-1]; count++; }

                p[x][y] = (sumP - divergence[x][y]) / count;
            }
        }
    }

    for (int x = 0; x < resolutionX; x++) {
        for (int y = 0; y < resolutionY; y++) {
            sf::Vector2f gradP(0.0f, 0.0f);
            if (x > 0 && x < resolutionX - 1 && !solid[x+1][y] && !solid[x-1][y])
                gradP.x = (p[x+1][y] - p[x-1][y]) / 2;
            if (y > 0 && y < resolutionY - 1 && !solid[x][y+1] && !solid[x][y-1])
                gradP.y = (p[x][y+1] - p[x][y-1]) / 2;
            velocities[x][y] = velocities[x][y] - gradP;
        }
    }

    for (int x = 0; x < resolutionX; x++) {
        delete[] divergence[x];
        delete[] p[x];
    }
    delete[] divergence;
    delete[] p;
}


float** Simulation::computeDivergence() {
    float** divergence = new float*[resolutionX];

    for (int x = 0; x < resolutionX; x++) {
        divergence[x] = new float[resolutionY];
        for (int y = 0; y < resolutionY; y++) {
            if (inert[x][y]) {
                divergence[x][y] = 0.0f;
                continue;
            }

            float vx1 = 0;
            if (x < resolutionX - 1) vx1 = velocities[x + 1][y].x;

            float vx2 = 0;
            if (x > 0) vx2 = velocities[x - 1][y].x;

            float vy1 = 0;
            if (y < resolutionY - 1) vy1 = velocities[x][y + 1].y;

            float vy2 = 0;
            if (y > 0) vy2 = velocities[x][y - 1].y;

            divergence[x][y] = (vx1 - vx2)/2 + (vy1 - vy2)/2;
        }
    }

    return divergence;
}

void Simulation::setInflow() {
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < resolutionY; y++) {
            velocities[x][y].x = inflowSpeed;
        }
    }
}

void Simulation::setSolidBlock(int x1, int y1, int x2, int y2) {
    for (int x = x1; x <= x2; x++) {
        for (int y = y1; y <= y2; y++) {
            inert[x][y] = true;
            solid[x][y] = true;
        }
    }
}

void Simulation::setSolidTriangle(int x1, int y1, int l1, int h, int l2) {
    float slope1 = h / static_cast<float>(l1);
    float slope2 = -h / static_cast<float>(l2);

    for (int i = 0; i <= l1; i++) {
        for (int j = 0; j <= std::ceil(i * slope1); j++) {
            int x = x1 + i;
            int y = y1 - j;
            inert[x][y] = true;
            solid[x][y] = true;
        }
    }

    for (int i = 0; i <= l2; i++) {
        for (int j = 0; j <= std::ceil(h + i * slope2); j++) {
            int x = x1 + l1 + i;
            int y = y1 - j;
            inert[x][y] = true;
            solid[x][y] = true;
        }
    }
}

void Simulation::setSolidCircle(int centerX, int centerY, int radius) {
    for (int x = centerX - radius; x <= centerX + radius; x++) {
        for (int y = centerY - radius; y <= centerY + radius; y++) {
            if (pow(centerX - x, 2) + pow(centerY - y, 2) < radius*radius) {
                inert[x][y] = true;
                solid[x][y] = true;
            }
        }
    }
}
