#include <iostream>
#include "Building.hpp"

sf::Texture Building::tileset;

bool Building::loadTexture(const std::string& path) {
    return tileset.loadFromFile(path);
}

void placeBuilding(BuildingType type, int tileX, int tileY,
    std::vector<Building>& buildings)
{
    if (type == BuildingType::None) return;

    float tileSize = 16.f;
    sf::Sprite sprite = getBuildingSprite(type);
    if (sprite.getTextureRect().position.x == 0 && sprite.getTextureRect().position.y == 0) {
        std::cout << "here" << std::endl;
        return;
    }

    sprite.setPosition({ tileX * tileSize, tileY * tileSize });

    buildings.push_back(Building{ type, sprite, {tileX, tileY} });
}

sf::Sprite getBuildingSprite(BuildingType type, int tileSize) {
    sf::Sprite icon(Building::tileset);

    switch (type) {
    case BuildingType::Barracks:
        icon.setTextureRect(sf::IntRect({ 8 * tileSize, 0 * tileSize }, { tileSize, tileSize }));
        break;
    case BuildingType::Farm:
        icon.setTextureRect(sf::IntRect({ 5 * tileSize, 2 * tileSize }, { tileSize, tileSize }));
        break;
    default:
        icon.setTextureRect(sf::IntRect({ 0, 0 }, { 0, 0 })); // invalid
        break;
    }

    return icon;
}

