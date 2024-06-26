#include <SFML/Graphics.hpp>
#include <../../../../libs/imgui/imgui.h>
#include <../../../../libs/imgui-sfml/imgui-SFML.h>
#include <random>

void render(sf::RenderWindow* window, int width, int height, sf::Color* color);

sf::Color randomColor();

int randint(int lowerBound, int upperBound);

int main() {
    const int width = 600;
    const int height = 600;

    sf::Color color = randomColor();

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

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Hello, ImGui!");

        if (ImGui::Button("Change Color")) {
            color = randomColor();
        }

        ImGui::End();

        window.clear();

        render(&window, width, height , &color);

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

void render(sf::RenderWindow* window, const int width, const int height, sf::Color* color) {
    sf::Image image;
    image.create(width, height);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            image.setPixel(x, y, *color);
        }
    }

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    window->draw(sprite);
}

sf::Color randomColor() {
    const sf::Color color(randint(0, 255), randint(0, 255), randint(0, 255));
    return color;
}

int randint(const int lowerBound, const int upperBound) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distrib(lowerBound, upperBound);

    return distrib(gen);
}