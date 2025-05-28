#pragma once
#include <SFML/Graphics.hpp>

enum class UnitType {
	Farmer,
	Soldier
};

sf::IntRect getUnitRect(UnitType type, int tileSize = 16);

struct Unit {
	sf::Sprite sprite;
	sf::Vector2i tilePos;
	sf::Vector2i targetTile; // where it's going
	float speed = 40.f;	// how fast it moves
	UnitType type;

	Unit(UnitType type, const sf::Texture& texture, sf::Vector2i startTile)
		: tilePos(startTile), targetTile(startTile), speed(40.f), type(type), sprite(texture)
	{
		sprite.setTextureRect(getUnitRect(type, 16));
		sprite.setPosition({ startTile.x * 16.f, startTile.y * 16.f }); // adjust for tile size
	}
};