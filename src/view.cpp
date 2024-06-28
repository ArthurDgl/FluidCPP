//
// Created by arthu on 6/28/2024.
//

#include "view.h"

#include "SFML/Graphics/View.hpp"

view::view(int mapSize, float *valueMap, sf::Color *colorMap) {
    this->mapSize = mapSize;
    this->valueMap = valueMap;
    this->colorMap = colorMap;
}


void view::getColorAt(int x, int y, sf::Color* color) {
    float value = this->getValueAt(x, y);

    if (value <= this->valueMap[0]) {
        *color = this->colorMap[0];
        return;
    }

    if (value >= this->valueMap[this->mapSize - 1]) {
        *color = this->colorMap[this->mapSize - 1];
        return;
    }

    for (int i = 0; i < this->mapSize - 1; ++i) {
        if (value >= this->valueMap[i] && value <= this->valueMap[i + 1]) {
            float interFactor = (value - this->valueMap[i]) / (this->valueMap[i + 1] - this->valueMap[i]);
            color->r = static_cast<sf::Uint8>((1 - interFactor) * this->colorMap[i].r + interFactor * this->colorMap[i + 1].r);
            color->g = static_cast<sf::Uint8>((1 - interFactor) * this->colorMap[i].g + interFactor * this->colorMap[i + 1].g);
            color->b = static_cast<sf::Uint8>((1 - interFactor) * this->colorMap[i].b + interFactor * this->colorMap[i + 1].b);
            return;
        }
    }
}


// Density View
densityView::densityView(float **densities, int mapSize, float *valueMap, sf::Color *colorMap) : view(mapSize, valueMap, colorMap) {
    this->densities = densities;
}

float densityView::getValueAt(int x, int y) const {
    return this->densities[x][y];
}


// Velocity X View
vXView::vXView(sf::Vector2f **velocities, int mapSize, float *valueMap, sf::Color *colorMap) : view(mapSize, valueMap, colorMap) {
    this->velocities = velocities;
}

float vXView::getValueAt(int x, int y) const {
    return this->velocities[x][y].x;
}
