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

    Building building(type, { tileX, tileY });
    buildings.push_back(building);
}

sf::Sprite getBuildingSprite(BuildingType type, int tileSize) {
    sf::Sprite icon(Building::tileset);

    icon.setTextureRect(getBuildingTileRect(type, tileSize));

    return icon;
}

sf::IntRect getBuildingTileRect(BuildingType type, int tileSize) {
    switch (type) {
    case BuildingType::Barracks:
        return sf::IntRect({ 8 * tileSize, 0 * tileSize }, { tileSize, tileSize });
    case BuildingType::Farm:
        return sf::IntRect({ 5 * tileSize, 2 * tileSize }, { tileSize, tileSize });
    default:
        return sf::IntRect({ 0, 0 }, { 0, 0 }); // invalid
    }
}