#include "Unit.hpp"
#include "Building.hpp"
#include "Tilemap.hpp"
#include <iostream>

Unit::Unit(UnitType type, const sf::Texture& texture, sf::Vector2i startTile)
    : tilePos(startTile), targetTile(startTile), speed(40.f), type(type), sprite(texture)
{
    sprite.setTextureRect(getUnitRect(type, 16));
    sprite.setPosition({ startTile.x * 16.f, startTile.y * 16.f }); // adjust for tile size

}
sf::Sprite getUnitSprite(UnitType type, int tileSize) {
    sf::Sprite icon(Building::tileset);

    icon.setTextureRect(getUnitRect(type, tileSize));

    return icon;
}

sf::IntRect getUnitRect(UnitType type, int tileSize) {
    switch (type) {
    case UnitType::Farmer:
        return sf::IntRect({ 17 * tileSize, 7 * tileSize }, { tileSize, tileSize });
    case UnitType::EnemySoldier:
        return sf::IntRect({ 16 * tileSize, 8 * tileSize }, { tileSize, tileSize });
    default:
        return sf::IntRect({ 0, 0 }, { 0, 0 }); // invalid
    }
}

bool spawnUnitNextToBuilding(Building& building, std::vector<Unit>& units, Tilemap& map) {
    static const std::vector<sf::Vector2i> offsets = {
        { 1, 0 }, {-1, 0 }, { 0, 1 }, { 0, -1 },
        { 1, 1 }, {-1, 1 }, { 1, -1 }, {-1, -1 }
    };

    for (const auto& offset : offsets) {
        sf::Vector2i spawnTile = building.tilePosition + offset;
        if (!map.isOccupied(spawnTile.x, spawnTile.y)) {
            Unit newUnit(building.buildQueue.front().unitType, Building::tileset, spawnTile);
            units.push_back(newUnit);
            map.markOccupied(spawnTile.x, spawnTile.y);
            return true; // spawned successfully
        }
    }

    return false; // no space to spawn
}