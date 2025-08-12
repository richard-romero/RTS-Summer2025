#include "Unit.hpp"
#include "Building.hpp"
#include "Tilemap.hpp"
#include <iostream>

Unit::Unit(UnitType type, const sf::Texture& texture, sf::Vector2i startTile)
    : tilePos(startTile), targetTile(startTile), speed(40.f), type(type), sprite(texture)
{
    sprite.setTextureRect(getUnitRect(type, 16));
    sprite.setPosition({ startTile.x * 16.f, startTile.y * 16.f }); // adjust for tile size

    switch (type) {
    case UnitType::Farmer:
        hp = 30;
        damage = 5;
        break;
    case UnitType::Soldier:
        hp = 100;
        damage = 20;
        break;
    case UnitType::EnemySoldier:
        hp = 80;
        damage = 15;
        break;
    }

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

int getUnitCost(UnitType type) {
    switch (type) {
    case UnitType::Farmer: return 25;
    case UnitType::Soldier: return 50;
    default: return 0;
    }
}

float distanceBetween(const sf::Vector2i& a, const sf::Vector2i& b) {
    return std::hypot(b.x - a.x, b.y - a.y);
}

Unit* findNearestPlayerUnit(const Unit& enemy, std::vector<Unit>& playerUnits) {
    Unit* nearest = nullptr;
    float nearestDist = std::numeric_limits<float>::max();

    for (auto& player : playerUnits) {
        float dist = distanceBetween(enemy.tilePos, player.tilePos);
        if (dist < nearestDist) {
            nearestDist = dist;
            nearest = &player;
        }
    }
    return nearest;
}