#pragma once
#include <vector>
#include "Unit.hpp"
#include "Tilemap.hpp"

struct EnemyWave {
	float spawnTime;
	int numEnemies;
};

class EnemyWaveSystem {
public:
	EnemyWaveSystem();

	void update(float dt, std::vector<Unit>& enemyUnits, const sf::Texture& tileset, Tilemap& map, sf::Vector2i targetTile);

	bool allWavesSpawned() const;

private:
	std::vector<EnemyWave> waves;
	int currentWaveIndex = 0;
	float waveTimer = 0.f;

	void spawnWave(int numEnemies, std::vector<Unit>& enemyUnits, const sf::Texture& tileset, Tilemap& map, sf::Vector2i targetTile);
};