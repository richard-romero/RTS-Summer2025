#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Building.hpp"
#include "Tilemap.hpp"
#include "UnitType.hpp"


sf::IntRect getUnitRect(UnitType type, int tileSize = 16);
sf::Sprite getUnitSprite(UnitType type, int tileSize);

struct Unit {
	sf::Sprite sprite;
	sf::Vector2i tilePos;
	sf::Vector2i targetTile; // where it's going
	float speed = 40.f;	// how fast it moves
	UnitType type;
	float chopTimer = 0.f; // time spent chopping tree

	Unit(UnitType type, const sf::Texture& texture, sf::Vector2i startTile);
};

bool spawnUnitNextToBuilding(Building& building, std::vector<Unit>& units, TileMap& map);