#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include "Unit.hpp"

enum class BuildingType {
	None,
	Barracks,
	Farm
};

sf::Sprite getBuildingSprite(BuildingType type, int tileSize = 16);

sf::IntRect getBuildingTileRect(BuildingType type, int tileSize = 16);

struct Building {
	static sf::Texture tileset;
	BuildingType type;
	sf::Sprite sprite;
	sf::Vector2i tilePosition;

	struct UnitBuildTask {
		float timeRemaining;
		UnitType unitType;
	};

	std::queue<UnitBuildTask> buildQueue;
	float buildTimer = 0.f;
	static bool loadTexture(const std::string& path);

	Building(BuildingType type, sf::Vector2i startTile)
		: tilePosition(startTile), type(type), sprite(Building::tileset)
	{
		sprite.setTextureRect(getBuildingTileRect(type, 16));
		sprite.setPosition({ startTile.x * 16.f, startTile.y * 16.f }); // adjust for tile size
	}
};

void placeBuilding(BuildingType type, int tileX, int tileY, std::vector<Building>& buildings);