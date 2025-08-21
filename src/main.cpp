#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono> 
#include "Tilemap.hpp"
#include "Building.hpp"
#include "Unit.hpp"
#include "Tree.hpp"
#include "EnemyWaveSystem.hpp"

struct Resources {
    int gold = 50;
    int wood = 80;
};

int main()
{
    // create the window
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int screenWidth = desktop.size.x;
    unsigned int screenHeight = desktop.size.y;

    float ratio = 0.75f;
    unsigned int windowWidth = static_cast<unsigned>(screenWidth * ratio);
    unsigned int windowHeight = static_cast<unsigned>(screenHeight * ratio);

    sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "RTS Game");

    window.setMouseCursorGrabbed(true);

    sf::View worldView({ 800.f, 800.f }, { 355.f, 200.f });

    bool gameOver = false;

    // define the level with an array of tile indices
    std::vector<int> level(100 * 100, 1);
    int tileSize = 16;


    //initialize tree vector
    std::vector<Tree> trees;

    //initialize enemy vector
    std::vector<Unit> enemyUnits;
    EnemyWaveSystem waveSystem;

    std::vector<size_t> deadEnemies;
    std::vector<size_t> deadPlayers;

    // create the tilemap from the level definition
    Tilemap map;
    if (!map.load("tilemap_packed.png", { 16, 16 }, level.data(), 100, 100))
        return -1;

    // create ui panel
    sf::RectangleShape uiPanel;
    uiPanel.setSize({ window.getSize().x * 0.25f, 100.f });
    uiPanel.setOrigin({ uiPanel.getSize().x / 2.f, 0.f }); // center it horizontally
    uiPanel.setPosition({ window.getSize().x / 2.f, window.getSize().y - 150.f });
    uiPanel.setFillColor(sf::Color(50, 50, 50, 200)); // semi-transparent gray

    // resources panel
    sf::RectangleShape resourcePanel({ 120.f, 50.f }); 
    resourcePanel.setPosition({ 20.f, window.getSize().y - 800.f });
    resourcePanel.setFillColor(sf::Color(50, 50, 50, 200));

    // create ui button for buildings
    sf::RectangleShape trainButton({ 120.f, 30.f });
    trainButton.setPosition({ 20.f, window.getSize().y - 50.f });
    trainButton.setFillColor(sf::Color::Blue);

    sf::Font font;
    if (!font.openFromFile("font.ttf")) {
        std::cerr << "Font not present\n";
        return -1;
    }

    sf::Text trainFarmerText(font, "Train Farmer", 16);
    trainFarmerText.setPosition({ trainButton.getPosition().x + 5, trainButton.getPosition().y + 5 });
    trainFarmerText.setFillColor(sf::Color::White);
    trainFarmerText.setFillColor(sf::Color::White);

    sf::Text trainSoldierText(font, "Train Soldier", 16);
    trainSoldierText.setPosition({ trainButton.getPosition().x + 5, trainButton.getPosition().y + 5 });
    trainSoldierText.setFillColor(sf::Color::White);
    trainSoldierText.setFillColor(sf::Color::White);


    // create resources struct and add number to text
    Resources resource;
    std::string label = "Gold: " + std::to_string(resource.gold) + "\nWood: " + std::to_string(resource.wood);
    sf::Text resourceText(font, label, 20);
    resourceText.setPosition({ resourcePanel.getPosition().x + 5, resourcePanel.getPosition().y + 5 });
    resourceText.setFillColor(sf::Color::White);

    // game over text
    sf::Text gameOverText(font, "Game Over", 48);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setOutlineColor(sf::Color::Black);
    gameOverText.setOutlineThickness(2.f);
    gameOverText.setPosition({ window.getSize().x / 2.f - 120.f, window.getSize().y / 2.f - 50.f });


    // create farm icon
    sf::Texture farmTexture("tilemap_packed.png");
    sf::Sprite farmIcon(farmTexture);
    farmIcon.setTextureRect(getBuildingTileRect(BuildingType::Farm));
    farmIcon.setScale({ 5.f, 5.f });
    farmIcon.setPosition({ 600.f, window.getSize().y - 140.f });

    // create barracks icon
    sf::Texture barracksTexture("tilemap_packed.png");
    sf::Sprite barracksIcon(barracksTexture);
    barracksIcon.setTextureRect(getBuildingTileRect(BuildingType::Barracks));
    barracksIcon.setScale({ 5.f, 5.f });
    barracksIcon.setPosition({ 760.f, window.getSize().y - 140.f });

    BuildingType selectedBuilding = BuildingType::None;

    float currentZoom = 1.0f;
    const float minZoom = 0.5f;
    const float maxZoom = 2.0f;

    // placed buildings vector
    std::vector<Building> placedBuildings;
    placeBuilding(BuildingType::Base, 50, 50, placedBuildings);
    map.markOccupied(50, 50);
    int baseIndex = placedBuildings.size() - 1;

    if (!Building::loadTexture("tilemap_packed.png")) {
        std::cerr << "Failed to load building tileset.\n";
        return -1;
    }

    // place trees diagonally
    for (int i = 0; i < 100; ++i) {
        trees.emplace_back(Building::tileset, sf::Vector2i({ i, i }));
    }

    // track units and buildings and if they are selected
    std::vector<Unit> units;
    int selectedUnitIndex = -1; // none selected
    int selectedBuildingIndex = -1;

    // hovering sprite
    std::optional<sf::Sprite> ghostSprite;

    // keep track of game time
    using clock = std::chrono::high_resolution_clock;
    auto lastFrameTime = clock::now();

    // run the main loop
    while (window.isOpen())
    {
        // handle events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }

            if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (mouseWheelScrolled->wheel == sf::Mouse::Wheel::Vertical) {
                    

                    if (mouseWheelScrolled->delta > 0 && currentZoom > minZoom) {
                        worldView.zoom(0.9f);
                        currentZoom *= 0.9f;
                    }
                    else if (mouseWheelScrolled->delta < 0 && currentZoom < maxZoom) {
                        worldView.zoom(1.1f);
                        currentZoom *= 1.1f;
                    }
                }
            }
            if (!gameOver) {
                if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseButtonPressed->button == sf::Mouse::Button::Left) {

                        // get pointer position
                        sf::Vector2i mouse = sf::Mouse::getPosition(window);
                        sf::Vector2f uiPos = window.mapPixelToCoords(mouse);
                        sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), worldView);

                        //select training box to queue unit
                        if (trainButton.getGlobalBounds().contains(uiPos)) {
                            if (selectedBuildingIndex != -1 &&
                                placedBuildings[selectedBuildingIndex].type == BuildingType::Farm) {
                                Building& b = placedBuildings[selectedBuildingIndex];

                                if (resource.gold >= getUnitCost(UnitType::Farmer)) {
                                    resource.gold -= getUnitCost(UnitType::Farmer);
                                    // Queue unit
                                    Building::UnitBuildTask task;
                                    task.unitType = UnitType::Farmer;
                                    task.timeRemaining = 1.0f;
                                    b.buildQueue.push(task);
                                    std::cout << "Queued Farmer\n";

                                    std::string label = "Gold: " + std::to_string(resource.gold) + "\nWood: " + std::to_string(resource.wood);
                                    resourceText.setString(label);
                                }
                                else {
                                    std::cout << "Not enough gold!\n";
                                }
                            }
                            else if (selectedBuildingIndex != -1 &&
                                placedBuildings[selectedBuildingIndex].type == BuildingType::Barracks) {
                                Building& b = placedBuildings[selectedBuildingIndex];

                                if (resource.gold >= getUnitCost(UnitType::Soldier)) {
                                    resource.gold -= getUnitCost(UnitType::Soldier);
                                    // Queue unit
                                    Building::UnitBuildTask task;
                                    task.unitType = UnitType::Soldier;
                                    task.timeRemaining = 1.0f;
                                    b.buildQueue.push(task);
                                    std::cout << "Queued Soldier\n";

                                    std::string label = "Gold: " + std::to_string(resource.gold) + "\nWood: " + std::to_string(resource.wood);
                                    resourceText.setString(label);
                                }
                                else {
                                    std::cout << "Not enough gold!\n";
                                }

                            }
                        }

                        // select buildings
                        selectedBuildingIndex = -1;
                        for (size_t i = 0; i < placedBuildings.size(); ++i) {
                            if (placedBuildings[i].sprite.getGlobalBounds().contains(worldPos)) {
                                selectedBuildingIndex = static_cast<int>(i);
                            }
                        }

                        // ui building interaction
                        if (farmIcon.getGlobalBounds().contains(uiPos)) {
                            selectedBuilding = BuildingType::Farm;
                            ghostSprite.emplace(Building::tileset);
                            ghostSprite->setTextureRect(getBuildingTileRect(selectedBuilding));
                            ghostSprite->setColor(sf::Color(255, 255, 255, 128));
                        } 
                        if (barracksIcon.getGlobalBounds().contains(uiPos)) {
                            selectedBuilding = BuildingType::Barracks;
                            ghostSprite.emplace(Building::tileset);
                            ghostSprite->setTextureRect(getBuildingTileRect(selectedBuilding));
                            ghostSprite->setColor(sf::Color(255, 255, 255, 128));
                        }

                        // building placement
                        if (selectedBuilding != BuildingType::None &&
                            mouse.y < window.getSize().y - 150) {
                            sf::Vector2f worldPos = window.mapPixelToCoords(mouse, worldView);
   
                            // Snap to tile grid
                            int tileX = static_cast<int>(std::floor(worldPos.x / tileSize));
                            int tileY = static_cast<int>(std::floor(worldPos.y / tileSize));

                            if (!map.isOccupied(tileX, tileY)) {
                                if (resource.wood >= getBuildingCost(selectedBuilding)) {
                                    resource.wood -= getBuildingCost(selectedBuilding);

                                    // Place your building here — e.g. spawn building sprite
                                    placeBuilding(selectedBuilding, tileX, tileY, placedBuildings);
                                    map.markOccupied(tileX, tileY);

                                    // adjust resources upon placement
                                    std::string label = "Gold: " + std::to_string(resource.gold) + "\nWood: " + std::to_string(resource.wood);
                                    resourceText.setString(label);
                                }
                                else {
                                std::cout << "Not enough gold!" << std::endl;
                                }
                            }

                            ghostSprite.reset();
                            selectedBuilding = BuildingType::None; // reset selection
                            continue;
                        }

                        // unit selection for movement
                        selectedUnitIndex = -1;
                        for (size_t i = 0; i < units.size(); ++i) {
                            if (units[i].sprite.getGlobalBounds().contains(worldPos)) {
                                selectedUnitIndex = static_cast<int>(i);
                                break;
                            }
                        }
                    }

                    if (mouseButtonPressed->button == sf::Mouse::Button::Right) {

                        // unit movement
                        if (selectedUnitIndex != -1) {
                            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
                            sf::Vector2f worldPos = window.mapPixelToCoords(mousePixelPos, worldView);

                            sf::Vector2i targetTile = {
                                static_cast<int>(worldPos.x) / 16,
                                static_cast<int>(worldPos.y) / 16,
                            };

                            units[selectedUnitIndex].targetTile = targetTile;
                        }
                    }
                }
            }
            
        }

        if (!gameOver) {
            auto now = clock::now();
            std::chrono::duration<float> elapsed = now - lastFrameTime;
            float dt = elapsed.count(); // in seconds
            lastFrameTime = now;

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            const int edgeMargin = 20;
            const float moveSpeed = 0.2f;

            sf::Vector2f offset(0.f, 0.f);

            if (mousePos.x < edgeMargin)
                offset.x -= moveSpeed;
            else if (mousePos.x > window.getSize().x - edgeMargin)
                offset.x += moveSpeed;

            if (mousePos.y < edgeMargin)
                offset.y -= moveSpeed;
            else if (mousePos.y > window.getSize().y - edgeMargin)
                offset.y += moveSpeed;

            worldView.move(offset);
            clampView(worldView, { 100, 100 }, { 16, 16 }, window.getSize());
        
            if (ghostSprite && selectedBuilding != BuildingType::None) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, worldView); // use current world view

                int tileX = static_cast<int>(std::floor(worldPos.x / tileSize));
                int tileY = static_cast<int>(std::floor(worldPos.y / tileSize));

                ghostSprite->setPosition({ tileX * static_cast<float>(tileSize), tileY * static_cast<float>(tileSize) });
            }  

            // update wave system
            waveSystem.update(dt, enemyUnits, Building::tileset, map, placedBuildings[baseIndex].tilePosition); // <- your actual base position

            // enemy movement + combat
            for (size_t e = 0; e < enemyUnits.size(); ++e) {
                Unit& enemy = enemyUnits[e];

                // Find nearest player unit
                Unit* nearestTarget = findNearestPlayerUnit(enemy, units);
                float attackRange = 2.0f; // in tiles
                bool attacked = false;

                // Attack player unit if in range
                if (nearestTarget) {
                    float distToPlayer = distanceBetween(enemy.tilePos, nearestTarget->tilePos);
                    if (nearestTarget->sprite.getColor() == sf::Color::Red) {
                        nearestTarget->flashTimer += dt;
                        if (nearestTarget->flashTimer >= 0.1f) {
                            nearestTarget->sprite.setColor(sf::Color::White);
                        }
                    }
                    if (distToPlayer <= attackRange) {
                        attacked = true;
                        enemy.attackTimer += dt;
                        if (enemy.attackTimer >= 1.f) {
                            nearestTarget->sprite.setColor(sf::Color::Red);
                            nearestTarget->flashTimer = 0.f;
                            nearestTarget->hp -= enemy.damage;
                            enemy.attackTimer = 0.f;

                            std::cout << "Player unit hit! HP: " << nearestTarget->hp << "\n";

                            if (nearestTarget->hp <= 0) {
                                // Mark player for removal
                                auto it = std::find_if(units.begin(), units.end(),
                                    [&](const Unit& u) { return &u == nearestTarget; });
                                if (it != units.end()) {
                                    deadPlayers.push_back(std::distance(units.begin(), it));
                                }
                            }
                        }
                    }
                }

                //If no player in range, attack base if in range
                if (!attacked) {
                    float distToBase = distanceBetween(enemy.tilePos, placedBuildings[baseIndex].tilePosition);

                    if (placedBuildings[baseIndex].sprite.getColor() == sf::Color::Red) {
                        placedBuildings[baseIndex].flashTimer += dt;
                        if (placedBuildings[baseIndex].flashTimer >= 0.1f) {
                            placedBuildings[baseIndex].sprite.setColor(sf::Color::White);
                        }
                    }
                    if (distToBase <= attackRange) {
                        enemy.attackTimer += dt;
                        if (enemy.attackTimer >= 1.f) {
                            placedBuildings[baseIndex].sprite.setColor(sf::Color::Red);
                            placedBuildings[baseIndex].flashTimer = 0.f;
                            placedBuildings[baseIndex].hp -= enemy.damage;
                            enemy.attackTimer = 0.f;

                            std::cout << "Base took damage! HP: " << placedBuildings[baseIndex].hp << "\n";
                            if (placedBuildings[baseIndex].hp <= 0) {
                                gameOver = true;
                            }
                        }
                        attacked = true;
                    }
                } 

                // Move toward target if not attacking
                if (!attacked) {
                    sf::Vector2i targetTile;
                    if (nearestTarget) {
                        targetTile = nearestTarget->tilePos;
                    }
                    else {
                        targetTile = placedBuildings[baseIndex].tilePosition;
                    }

                    sf::Vector2f targetPos(
                        targetTile.x * tileSize,
                        targetTile.y * tileSize
                    );
                    sf::Vector2f currentPos = enemy.sprite.getPosition();
                    sf::Vector2f delta = targetPos - currentPos;
                    float distPixels = std::hypot(delta.x, delta.y);

                    if (distPixels > 0.f) {
                        sf::Vector2f direction = delta / distPixels;
                        enemy.sprite.move(direction * enemy.speed * dt);

                        // Update tile position
                        enemy.tilePos = sf::Vector2i(
                            static_cast<int>(enemy.sprite.getPosition().x) / tileSize,
                            static_cast<int>(enemy.sprite.getPosition().y) / tileSize
                        );
                    }
                }
            }

            // player unit combat
            for (size_t u = 0; u < units.size(); ++u) {
                Unit& unit = units[u];

                // Find nearest enemy unit
                Unit* nearestTarget = findNearestPlayerUnit(unit, enemyUnits);
                float attackRange = 2.0f; // in tiles
                bool attacked = false;

                // Attack unit if in range
                if (nearestTarget) {
                    float distToPlayer = distanceBetween(unit.tilePos, nearestTarget->tilePos);
                    if (nearestTarget->sprite.getColor() == sf::Color::Red) {
                        nearestTarget->flashTimer += dt;
                        if (nearestTarget->flashTimer >= 0.1f) {
                            nearestTarget->sprite.setColor(sf::Color::White);
                        }
                    }
                    if (distToPlayer <= attackRange) {
                        attacked = true;
                        unit.attackTimer += dt;
                        if (unit.attackTimer >= 1.f) {
                            nearestTarget->sprite.setColor(sf::Color::Red);
                            nearestTarget->flashTimer = 0.f;
                            nearestTarget->hp -= unit.damage;
                            unit.attackTimer = 0.f;

                            std::cout << "Player unit hit! HP: " << nearestTarget->hp << "\n";

                            if (nearestTarget->hp <= 0) {
                                // Mark enemy for removal
                                auto it = std::find_if(enemyUnits.begin(), enemyUnits.end(),
                                    [&](const Unit& u) { return &u == nearestTarget; });
                                if (it != enemyUnits.end()) {
                                    deadEnemies.push_back(std::distance(enemyUnits.begin(), it));
                                }
                            }
                        }
                    }
                }
            }

            // Award gold & remove dead enemies
            std::sort(deadEnemies.rbegin(), deadEnemies.rend());
            for (size_t idx : deadEnemies) {
                resource.gold += 10; // example gold reward
                std::cout << resource.gold << std::endl;
                std::string label = "Gold: " + std::to_string(resource.gold) + "\nWood: " + std::to_string(resource.wood);
                resourceText.setString(label);
                enemyUnits.erase(enemyUnits.begin() + idx);
            }
            deadEnemies.clear();

            // Remove dead players
            std::sort(deadPlayers.rbegin(), deadPlayers.rend());
            for (size_t idx : deadPlayers) {
                units.erase(units.begin() + idx);
            }
            deadPlayers.clear();

            // place queued units
            for (auto& building : placedBuildings) {
                if (!building.buildQueue.empty()) {
                    auto& task = building.buildQueue.front();
                    task.timeRemaining -= dt;
                    if (task.timeRemaining <= 0.f) {
                        // spawn unit
                        std::cout << "unit complete\n";
                        if (spawnUnitNextToBuilding(building, units, map)) {
                            building.buildQueue.pop(); // only pop if successfully spawned
                        }
                    }
                }
            }

            // move units
            for (auto& unit : units) {
                sf::Vector2f currentPos = unit.sprite.getPosition();
                sf::Vector2f targetPos = {
                    unit.targetTile.x * static_cast<float>(tileSize),
                    unit.targetTile.y * static_cast<float>(tileSize)
                };

                sf::Vector2f delta = targetPos - currentPos;
                float distance = std::hypot(delta.x, delta.y);

                if (distance > 1.f) {
                    sf::Vector2f direction = delta / distance;
                    unit.sprite.move(direction * unit.speed * dt);
                }
                else {
                    unit.tilePos = unit.targetTile;
                }
            }

            // chop tree 
            for (auto& unit : units) {
                if (unit.type != UnitType::Farmer) continue;

                bool chopping = false;

                for (auto& tree : trees) {
                    if (tree.isChopped) continue;

                    if (unit.tilePos == tree.tilePos) {
                        unit.chopTimer += dt;
                        chopping = true;

                        if (unit.chopTimer >= 1.0f) {
                            tree.chop(10); // 10 wood per frame
                            resource.wood += 10;
                            std::string label = "Gold: " + std::to_string(resource.gold) + "\nWood: " + std::to_string(resource.wood);
                            resourceText.setString(label);
                            unit.chopTimer = 0.f; // reset timer every second

                        }

                        break; // only chop one tree at a time
                    }
                }
                if (!chopping) {
                    unit.chopTimer = 0.f; // reset only if no matching tree
                }
            }

            units.erase(
                std::remove_if(units.begin(), units.end(),
                    [](const Unit& u) {
                        return u.hp <= 0;
                    }),
                units.end()
            );

            enemyUnits.erase(
                std::remove_if(enemyUnits.begin(), enemyUnits.end(),
                    [](const Unit& u) {
                        return u.hp <= 0;
                    }),
                enemyUnits.end()
            );

        }

        // draw the map
        window.clear();
        window.setView(worldView);
        window.draw(map);

        for (const Tree& tree : trees) {
            window.draw(tree.sprite);
        }

        for (const auto& building : placedBuildings)
            window.draw(building.sprite);

        for (const auto& unit : units) {
            window.draw(unit.sprite);
        }

        // draw enemies
        for (const auto& unit : enemyUnits) {
            window.draw(unit.sprite);
        }

        if (ghostSprite)
            window.draw(*ghostSprite);

        window.setView(window.getDefaultView());

        // farm menu
        if (selectedBuildingIndex != -1 && placedBuildings[selectedBuildingIndex].type == BuildingType::Farm &&
            !ghostSprite) {
            window.draw(trainButton);
            window.draw(trainFarmerText);
        }

        if (selectedBuildingIndex != -1 && placedBuildings[selectedBuildingIndex].type == BuildingType::Barracks &&
            !ghostSprite) {
            window.draw(trainButton);
            window.draw(trainSoldierText);
        }

        if (gameOver) {
            window.setView(window.getDefaultView());
            window.draw(gameOverText);
        }

        window.draw(uiPanel);
        window.draw(farmIcon);
        window.draw(barracksIcon);
        window.draw(resourcePanel);
        window.draw(resourceText);
        window.display();
    }
}