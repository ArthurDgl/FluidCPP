//
// Created by arthu on 6/28/2024.
//

#ifndef VIEW_H
#define VIEW_H

#include <SFML/Graphics/Color.hpp>

#include "SFML/System/Vector2.hpp"


class view {
public:
    int mapSize;
    float* valueMap;
    sf::Color* colorMap;

    view(int mapSize, float* valueMap, sf::Color* colorMap);

    virtual float getValueAt(int x, int y) const = 0;

    void getColorAt(int x, int y, sf::Color* color);
};

class densityView : public view {
public:
    float** densities;

    densityView(float** densities, int mapSize, float* valueMap, sf::Color* colorMap);

    float getValueAt(int x, int y) const override;
};

class vXView : public view {
public:
    sf::Vector2f** velocities;

    vXView(sf::Vector2f** velocities, int mapSize, float* valueMap, sf::Color* colorMap);

    float getValueAt(int x, int y) const override;
};


#endif //VIEW_H
