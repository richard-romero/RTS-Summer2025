#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include "Unit.hpp"

enum class BuildingType {
	None,
	Barracks,
	Farm,
	Base
};

struct Building {
	static sf::Texture tileset;
	BuildingType type;
	sf::Sprite sprite;
	sf::Vector2i tilePosition;
	float flashTimer = 0.f;

	struct UnitBuildTask {
		float timeRemaining;
		UnitType unitType;
	};

	std::queue<UnitBuildTask> buildQueue;
	float buildTimer = 0.f;
	static bool loadTexture(const std::string& path);
	int hp = 100; // base (default) hp

	Building(BuildingType type, sf::Vector2i startTile);
};

void placeBuilding(BuildingType type, int tileX, int tileY, std::vector<Building>& buildings);
sf::Sprite getBuildingSprite(BuildingType type, int tileSize = 16);
sf::IntRect getBuildingTileRect(BuildingType type, int tileSize = 16);
int getBuildingCost(BuildingType type);