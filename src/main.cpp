#include <iostream>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "Simulation.h"

struct Block {
    int x1;
    int y1;
    int x2;
    int y2;
};

void mapValueToColors(float value, const float inputValues[], const sf::Color outputColors[], int size, sf::Color* color);
void startSimulation(Simulation* simulation, int* cellSize, int width, int height, int resolutionX, int resolutionY, Block* block);

int main() {
    constexpr int width = 1200;
    constexpr int height = 800;

    float runSpeed = 1.0f;

    bool restartSimulation = false;
    int restartResolutionX = 150;
    int restartResolutionY = 50;
    Block block = {30, 33, 39, 49};

    Simulation* simulation;
    int cellSize;
    startSimulation(simulation, &cellSize, width, height, restartResolutionX, restartResolutionY, &block);

    auto window = sf::RenderWindow{ { width, height }, "FluidCPP" };
    window.setFramerateLimit(144);

    ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        for (auto event = sf::Event{}; window.pollEvent(event);) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        sf::Time deltaTime = deltaClock.getElapsedTime();
        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Real-Time Configuration");
        // ImGui code here
        float fps = std::round(100.0f/deltaTime.asSeconds()) / 100.0f;
        ImGui::Text((std::string("FPS : ") + std::to_string(fps)).c_str());
        ImGui::InputFloat("Time Intervals", &simulation->timeStep);
        if (simulation->timeStep < 0.01) simulation->timeStep = 0.01;
        ImGui::InputFloat("Real-Time Ratio", &runSpeed);

        ImGui::NewLine();

        ImGui::Text("Simulation Values");
        ImGui::InputFloat("Diffusion Strength", &simulation->diffusionStrength);
        ImGui::InputInt("Diffusion Steps /!\\*", &simulation->diffusionSteps);
        ImGui::InputInt("Divergence Steps /!\\*", &simulation->divergenceSteps);
        ImGui::InputFloat("Inflow Speed", &simulation->inflowSpeed);

        ImGui::NewLine();
        ImGui::Text("*/!\\ : Handle with care, can drop the FPS");
        ImGui::Text("Safety prevents FPS to go under 10 FPS by pausing the Simulation");

        ImGui::End();

        ImGui::Begin("Restart Configuration");

        restartSimulation = ImGui::Button("RESTART");

        ImGui::Text("Simulation Dimensions");
        ImGui::InputInt("ResolutionX", &restartResolutionX);
        ImGui::InputInt("ResolutionY", &restartResolutionY);

        ImGui::NewLine();

        ImGui::Text("Block Obstacle Dimensions");
        ImGui::InputInt("X1", &block.x1);
        ImGui::InputInt("Y1", &block.y1);
        ImGui::InputInt("X2", &block.x2);
        ImGui::InputInt("Y2", &block.y2);

        ImGui::End();

        if (restartSimulation) startSimulation(simulation, &cellSize, width, height, restartResolutionX, restartResolutionY, &block);

        simulation->tick(deltaTime.asSeconds()*runSpeed);

        window.clear();

        // SFML render code here
        float valueMap[] = {-10.0f, 0.0f, 15.0f, 25.0f};
        sf::Color colorMap[] = {sf::Color::Blue, sf::Color::Green, sf::Color::Yellow, sf::Color::Red};

        int mapSize = 4;

        sf::VertexArray rectangles(sf::Quads, simulation->resolutionX*simulation->resolutionY*4);
        sf::Color color = sf::Color::Black;

        for (int x = 0; x < simulation->resolutionX; x++) {
            for (int y = 0; y < simulation->resolutionY; y++) {
                float xPos = static_cast<float>(x * cellSize);
                float yPos = static_cast<float>(y * cellSize);

                int index = (x * simulation->resolutionY + y) * 4;
                rectangles[index].position = sf::Vector2f(xPos, yPos);
                rectangles[index + 1].position = sf::Vector2f(xPos + cellSize, yPos);
                rectangles[index + 2].position = sf::Vector2f(xPos + cellSize, yPos + cellSize);
                rectangles[index + 3].position = sf::Vector2f(xPos, yPos + cellSize);

                mapValueToColors(simulation->velocities[x][y].x, valueMap, colorMap, mapSize, &color);
                if (simulation->solid[x][y]) color = sf::Color(50, 50, 50);

                rectangles[index].color = color;
                rectangles[index + 1].color = color;
                rectangles[index + 2].color = color;
                rectangles[index + 3].color = color;
            }
        }
        window.draw(rectangles);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

void startSimulation(Simulation* simulation, int* cellSize, int width, int height, int resolutionX, int resolutionY, Block* block) {
    *simulation = *new Simulation(resolutionX, resolutionY);

    simulation->setSolidBlock(block->x1, block->y1, block->x2, block->y2);

    *cellSize = width / simulation->resolutionX;
    if (*cellSize > height / simulation->resolutionY) *cellSize = height / simulation->resolutionY;
}

void mapValueToColors(float value, const float inputValues[], const sf::Color outputColors[], int size, sf::Color* color) {
    if (value <= inputValues[0]) {
        *color = outputColors[0];
        return;
    }

    if (value >= inputValues[size - 1]) {
        *color = outputColors[size - 1];
        return;
    }

    for (int i = 0; i < size - 1; ++i) {
        if (value >= inputValues[i] && value <= inputValues[i + 1]) {
            float interFactor = (value - inputValues[i]) / (inputValues[i + 1] - inputValues[i]);
            color->r = static_cast<sf::Uint8>((1 - interFactor) * outputColors[i].r + interFactor * outputColors[i + 1].r);
            color->g = static_cast<sf::Uint8>((1 - interFactor) * outputColors[i].g + interFactor * outputColors[i + 1].g);
            color->b = static_cast<sf::Uint8>((1 - interFactor) * outputColors[i].b + interFactor * outputColors[i + 1].b);
            return;
        }
    }
}