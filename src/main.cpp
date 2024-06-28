#include <iostream>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "simulation.h"
#include "view.h"

struct Block {
    int x1;
    int y1;
    int x2;
    int y2;
};

struct SimulationParameters {
    int windowWidth;
    int windowHeight;
    int resolutionX;
    int resolutionY;

    Block* block;

    float timeStep;
    float runSpeed;

    float diffusionStrength;
    int diffusionSteps;

    int divergenceSteps;

    float inflowSpeed;
};

struct ViewParameters {

};

void mapValueToColors(float value, const float inputValues[], const sf::Color outputColors[], int size, sf::Color* color);
void startSimulation(Simulation* simulation, int* cellSize, SimulationParameters* parameters);
void applyParameters(Simulation* simulation, SimulationParameters* parameters);

int main() {
    constexpr int width = 1200;
    constexpr int height = 800;

    bool restartSimulation = false;

    Block block = {30, 20, 39, 30};

    SimulationParameters parameters = {
        width, height,
        200, 50,
        &block,
        0.05f, 1.0f,
        0.05, 10,
        200,
        20.0f
    };

    Simulation* simulation = new Simulation(1, 1);
    int cellSize;

    startSimulation(simulation, &cellSize, &parameters);
    applyParameters(simulation, &parameters);

    float valueMap1[] = {-10.0f, 0.0f, 15.0f, 25.0f};
    sf::Color colorMap1[] = {sf::Color::Blue, sf::Color::Green, sf::Color::Yellow, sf::Color::Red};
    vXView* velocityXView = new vXView(simulation->velocities, 4, valueMap1, colorMap1);

    float valueMap2[] = {0.0f, 0.33f, 0.66f, 1.0f};
    sf::Color colorMap2[] = {sf::Color::Blue, sf::Color::Green, sf::Color::Yellow, sf::Color::Red};
    densityView* dView = new densityView(simulation->densities, 4, valueMap2, colorMap2);

    view* views[] = {velocityXView, dView};
    const char* viewNames[] = { "Velocity X", "Density"};
    static int currentView = 0;
    int viewCount = 2;

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

        ImGui::Text("View");
        ImGui::Combo("Select View", &currentView, viewNames, viewCount);

        ImGui::NewLine();

        float fps = std::round(100.0f/deltaTime.asSeconds()) / 100.0f;
        ImGui::Text((std::string("FPS : ") + std::to_string(fps)).c_str());
        ImGui::InputFloat("Time Intervals", &parameters.timeStep);
        if (parameters.timeStep < 0.01) parameters.timeStep = 0.01;
        ImGui::InputFloat("Real-Time Ratio", &parameters.runSpeed);

        ImGui::NewLine();

        ImGui::Text("Simulation Values");
        ImGui::InputFloat("Diffusion Strength", &parameters.diffusionStrength);
        ImGui::InputInt("Diffusion Steps /!\\*", &parameters.diffusionSteps);
        ImGui::InputInt("Divergence Steps /!\\*", &parameters.divergenceSteps);
        ImGui::InputFloat("Inflow Speed", &parameters.inflowSpeed);

        ImGui::NewLine();
        ImGui::Text("*/!\\ : Handle with care, can drop the FPS");
        ImGui::Text("Safety prevents FPS to go under 10 FPS by pausing the Simulation");

        ImGui::End();

        applyParameters(simulation, &parameters);

        ImGui::Begin("Restart Configuration");

        restartSimulation = ImGui::Button("RESTART");

        ImGui::Text("Simulation Dimensions");
        ImGui::InputInt("ResolutionX", &parameters.resolutionX);
        ImGui::InputInt("ResolutionY", &parameters.resolutionY);

        ImGui::NewLine();

        ImGui::Text("Block Obstacle Dimensions");
        ImGui::InputInt("X1", &block.x1);
        ImGui::InputInt("Y1", &block.y1);
        ImGui::InputInt("X2", &block.x2);
        ImGui::InputInt("Y2", &block.y2);

        ImGui::End();

        if (restartSimulation) {
            startSimulation(simulation, &cellSize, &parameters);
            velocityXView->velocities = simulation->velocities;
            dView->densities = simulation->densities;
        }

        simulation->tick(deltaTime.asSeconds()*parameters.runSpeed);

        window.clear();

        // SFML render code here

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

                views[currentView]->getColorAt(x, y, &color);
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

void startSimulation(Simulation* simulation, int* cellSize, SimulationParameters* parameters) {
    *simulation = *new Simulation(parameters->resolutionX, parameters->resolutionY);

    simulation->setSolidBlock(parameters->block->x1, parameters->block->y1, parameters->block->x2, parameters->block->y2);

    *cellSize = parameters->windowWidth / simulation->resolutionX;
    if (*cellSize > parameters->windowHeight / simulation->resolutionY) *cellSize = parameters->windowHeight / simulation->resolutionY;
}

void applyParameters(Simulation *simulation, SimulationParameters *parameters) {
    simulation->diffusionStrength = parameters->diffusionStrength;
    simulation->diffusionSteps = parameters->diffusionSteps;
    simulation->divergenceSteps = parameters->divergenceSteps;
    simulation->timeStep = parameters->timeStep;
    simulation->inflowSpeed = parameters->inflowSpeed;
}


