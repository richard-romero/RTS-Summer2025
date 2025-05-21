#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum class BuildingType {
	None,
	Barracks,
	Farm
};

struct Building {
	static sf::Texture tileset;
	BuildingType type;
	sf::Sprite sprite;
	sf::Vector2i tilePosition;

	static bool loadTexture(const std::string& path);
};

void placeBuilding(BuildingType type, int tileX, int tileY, std::vector<Building>& buildings);

sf::Sprite getBuildingSprite(BuildingType type, int tileSize = 16);

sf::IntRect getBuildingTileRect(BuildingType type, int tileSize = 16);