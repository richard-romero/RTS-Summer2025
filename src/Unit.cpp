#include "Unit.hpp"

sf::IntRect getUnitRect(UnitType type, int tileSize) {
    switch (type) {
    case UnitType::Farmer:
        return sf::IntRect({ 6 * tileSize, 0 * tileSize }, { tileSize, tileSize });
    default:
        return sf::IntRect({ 0, 0 }, { 0, 0 }); // invalid
    }
}