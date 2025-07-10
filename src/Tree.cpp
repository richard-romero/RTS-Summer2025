#include "Tree.hpp"

Tree::Tree(const sf::Texture& tileset, sf::Vector2i tile) : tilePos(tile), sprite(tileset) {
	sprite.setTextureRect(sf::IntRect({ 4 * 16, 5 * 16 }, { 16, 16 }));
	sprite.setPosition({ tile.x * 16.f, tile.y * 16.f });
}

void Tree::chop(int amount) {
	if (isChopped) return;

	woodRemaining -= amount;
	if (woodRemaining < 0) woodRemaining = 0;

	// fade tree based on remaining wood
	float ratio = static_cast<float>(woodRemaining) / 100.f;
	int alpha = static_cast<int>(ratio * 255.f);
	sprite.setColor(sf::Color(255, 255, 255, alpha));

	if (woodRemaining == 0) {
		isChopped = true;
		
		// tree disappears
		sprite.setColor(sf::Color(100, 100, 100, 0));
	}

}