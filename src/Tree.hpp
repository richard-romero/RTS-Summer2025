#pragma once
#include <SFML/Graphics.hpp>

struct Tree {
	sf::Sprite sprite;
	sf::Vector2i tilePos;
	int woodRemaining = 100;
	bool isChopped = false;

	Tree(const sf::Texture& tileset, sf::Vector2i tile);

	void chop(int amount);
};