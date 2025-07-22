#include "EnemyWaveSystem.hpp"
#include <iostream>

EnemyWaveSystem::EnemyWaveSystem() {
    waves = {
        { 10.f, 3 },
        { 25.f, 5 },
        { 45.f, 8 },
        { 70.f, 12 }
    };
}

void EnemyWaveSystem::update(float dt, std::vector<Unit>& enemyUnits, const sf::Texture& tileset, Tilemap& map, sf::Vector2i targetTile) {
    if (currentWaveIndex >= waves.size()) return;

    waveTimer += dt;

    const EnemyWave& currentWave = waves[currentWaveIndex];
    if (waveTimer >= currentWave.spawnTime) {
        std::cout << "Spawning wave " << currentWaveIndex + 1 << " with " << currentWave.numEnemies << " enemies\n";
        spawnWave(currentWave.numEnemies, enemyUnits, tileset, map, targetTile);
        currentWaveIndex++;
    }
}

bool EnemyWaveSystem::allWavesSpawned() const {
    return currentWaveIndex >= waves.size();
}

void EnemyWaveSystem::spawnWave(int numEnemies, std::vector<Unit>& enemyUnits, const sf::Texture& tileset, Tilemap& map, sf::Vector2i targetTile) {
    sf::Vector2i spawnTile = { 0, 0 }; // top-left or a predefined spawn point

    for (int i = 0; i < numEnemies; ++i) {
        // Slight offset so they don't overlap exactly
        sf::Vector2i offset = { i % 3, i / 3 };
        sf::Vector2i pos = spawnTile + offset;

        if (!map.isOccupied(pos.x, pos.y)) {
            Unit enemy(UnitType::EnemySoldier, tileset, pos);
            // set initial target to base tile
            enemy.targetTile = targetTile;
            enemyUnits.push_back(enemy);
            map.markOccupied(pos.x, pos.y);
        }
    }
}