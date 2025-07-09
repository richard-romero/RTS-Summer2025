#include "Tree.hpp"

Tree::Tree(const sf::Texture& tileset, sf::Vector2i tile) : tilePos(tile), sprite(tileset) {
	sprite.setTextureRect(sf::IntRect({ 4 * 16, 5 * 16 }, { 16, 16 }));
	sprite.setPosition({ tile.x * 16.f, tile.y * 16.f });
}