#pragma once
#include <SFML/Graphics.hpp>

struct Tree {
	sf::Sprite sprite;
	sf::Vector2i tilePos;

	Tree(const sf::Texture& tileset, sf::Vector2i tile);
};