#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum class UnitType {
	Farmer,
	Soldier,
	EnemySoldier
};

class Building;
class Tilemap;

sf::IntRect getUnitRect(UnitType type, int tileSize = 16);
sf::Sprite getUnitSprite(UnitType type, int tileSize);

struct Unit {
	sf::Sprite sprite;
	sf::Vector2i tilePos;
	sf::Vector2i targetTile; // where it's going
	float speed = 40.f;	// how fast it moves
	UnitType type;
	float chopTimer = 0.f; // time spent chopping tree
	float attackTimer = 0.f; // time spent attacking
	int hp = 50; // default hp
	int damage = 10; //default damage done to other units/buildings

	Unit(UnitType type, const sf::Texture& texture, sf::Vector2i startTile);
};

bool spawnUnitNextToBuilding(Building& building, std::vector<Unit>& units, Tilemap& map);
int getUnitCost(UnitType type);
Unit* findNearestPlayerUnit(const Unit& enemy, std::vector<Unit>& playerUnits);
float distanceBetween(const sf::Vector2i& a, const sf::Vector2i& b);
