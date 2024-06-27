#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "Simulation.h"

void mapValueToColors(float value, const float inputValues[], const sf::Color outputColors[], int size, sf::Color* color);

int main() {
    constexpr int width = 600;
    constexpr int height = 600;

    auto simulation = new Simulation(100, 50);

    int cellSize = width / simulation->resolutionX;
    if (cellSize > height / simulation->resolutionY) cellSize = height / simulation->resolutionY;

    auto window = sf::RenderWindow{ { width, height }, "CMake SFML Project" };
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

        ImGui::Begin("Fluid Sim Configuration");
        // ImGui code here
        float fps = std::round(100.0f/deltaTime.asSeconds()) / 100.0f;
        ImGui::Text((std::string("FPS : ") + std::to_string(fps)).c_str());

        ImGui::End();

        simulation->tick(deltaTime.asSeconds());

        window.clear();

        // SFML render code here
        float valueMap[] = {0.0f, 0.5f, 1.0f};
        sf::Color colorMap[] = {sf::Color::Blue, sf::Color::Green, sf::Color::Yellow};
        int mapSize = 3;

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

                mapValueToColors(simulation->densities[x][y], valueMap, colorMap, mapSize, &color);
                if (simulation->inert[x][y]) color = sf::Color(50, 50, 50);

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