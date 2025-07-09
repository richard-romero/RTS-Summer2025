#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono> 
#include "Tilemap.hpp"
#include "Building.hpp"
#include "Unit.hpp"
#include "Tree.hpp"

struct Resources {
    int gold = 500;
    int wood = 200;
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

    // define the level with an array of tile indices
    std::vector<int> level(100 * 100, 1);


    //initialize tree vector
    std::vector<Tree> trees;

    // place trees diagonally
    for (int i = 0; i < 100; ++i) {
        trees.emplace_back(Building::tileset, sf::Vector2i({ i, i }));
    }

    // create the tilemap from the level definition
    TileMap map;
    if (!map.load("tilemap_packed.png", { 16, 16 }, level.data(), 100, 100))
        return -1;

    // create ui panel
    sf::RectangleShape uiPanel;
    uiPanel.setSize({ window.getSize().x * 0.75f, 100.f });
    uiPanel.setOrigin({ uiPanel.getSize().x / 2.f, 0.f }); // center it horizontally
    uiPanel.setPosition({ window.getSize().x / 2.f, window.getSize().y - 150.f });
    uiPanel.setFillColor(sf::Color(50, 50, 50, 200)); // semi-transparent gray

    // resources panel
    sf::RectangleShape resourcePanel({ 120.f, 30.f }); 
    resourcePanel.setPosition({ 20.f, window.getSize().y - 800.f });
    resourcePanel.setFillColor(sf::Color(50, 50, 50, 200));

    // create ui button for barracks
    sf::RectangleShape trainButton({ 120.f, 30.f });
    trainButton.setPosition({ 20.f, window.getSize().y - 50.f });
    trainButton.setFillColor(sf::Color::Blue);

    sf::Font font;
    if (!font.openFromFile("font.ttf")) {
        std::cerr << "Font not present\n";
        return -1;
    }

    sf::Text trainText(font, "Train Farmer", 16);
    trainText.setPosition({ trainButton.getPosition().x + 5, trainButton.getPosition().y + 5 });
    trainText.setFillColor(sf::Color::White);

    // create resources struct and add number to text
    Resources resource;
    std::string label = "Gold: " + std::to_string(resource.gold);
    sf::Text resourceText(font, label, 20);
    resourceText.setPosition({ resourcePanel.getPosition().x + 5, resourcePanel.getPosition().y + 5 });
    resourceText.setFillColor(sf::Color::White);


    // create barracks icon
    sf::Texture barracksTexture("tilemap_packed.png");
    sf::Sprite barracksIcon(barracksTexture);
    int tileSize = 16;
    int tileX = 8; // column
    int tileY = 0; // row
    barracksIcon.setTextureRect(sf::IntRect({ tileX * tileSize, tileY * tileSize }, { tileSize, tileSize }));
    barracksIcon.setScale({ 5.f, 5.f });
    barracksIcon.setPosition({ 250.f, window.getSize().y - 140.f });

    BuildingType selectedBuilding = BuildingType::None;

    float currentZoom = 1.0f;
    const float minZoom = 0.5f;
    const float maxZoom = 2.0f;

    // placed buildings vector
    std::vector<Building> placedBuildings;
    if (!Building::loadTexture("tilemap_packed.png")) {
        std::cerr << "Failed to load building tileset.\n";
        return -1;
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

            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {

                    // get pointer position
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    sf::Vector2f uiPos = window.mapPixelToCoords(mouse);
                    sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), worldView);

                    //select training box to queue unit
                    if (trainButton.getGlobalBounds().contains(uiPos)) {
                        if (selectedBuildingIndex != -1 && 
                            placedBuildings[selectedBuildingIndex].type == BuildingType::Barracks) {
                            Building& b = placedBuildings[selectedBuildingIndex];

                            // add task to build queue
                            Building::UnitBuildTask task;
                            task.unitType = UnitType::Farmer;
                            task.timeRemaining = 1.0f;

                            b.buildQueue.push(task);
                            std::cout << "Queued Farmer\n";

                        }
                    }

                    // select buildings
                    selectedBuildingIndex = -1;
                    for (size_t i = 0; i < placedBuildings.size(); ++i) {
                        if (placedBuildings[i].sprite.getGlobalBounds().contains(worldPos)) {
                            if (placedBuildings[i].type == BuildingType::Barracks) {
                                selectedBuildingIndex = static_cast<int>(i);
                            }
                            break;
                        }
                    }

                    // ui building interaction
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
                        int tileX = static_cast<int>(std::floor(worldPos.x / 16.f));
                        int tileY = static_cast<int>(std::floor(worldPos.y / 16.f));
                        
                        if (!map.isOccupied(tileX, tileY)) {
                            if (resource.gold < 100) {
                                std::cout << "Not enough gold!" << std::endl;
                            }
                            else {
                            // Place your building here — e.g. spawn building sprite
                            placeBuilding(selectedBuilding, tileX, tileY, placedBuildings);
                            map.markOccupied(tileX, tileY);

                            // adjust resources upon placement
                            resource.gold -= 100;
                            label = "Gold: " + std::to_string(resource.gold);
                            resourceText.setString(label);
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
                            static_cast<int>(worldPos.x) / tileSize,
                            static_cast<int>(worldPos.y) / tileSize,
                        };

                        units[selectedUnitIndex].targetTile = targetTile;
                    }
                }
            }
            
        }
        auto now = clock::now();
        std::chrono::duration<float> elapsed = now - lastFrameTime;
        float dt = elapsed.count(); // in seconds
        lastFrameTime = now;

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        const int edgeMargin = 20;
        const float moveSpeed = 0.1f;

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

            int tileX = static_cast<int>(std::floor(worldPos.x / 16.f));
            int tileY = static_cast<int>(std::floor(worldPos.y / 16.f));

            ghostSprite->setPosition({ tileX * 16.f, tileY * 16.f });
        }    

        // place queued units
        for (auto& building : placedBuildings) {
            if (building.type != BuildingType::Barracks) continue;

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
                        unit.chopTimer = 0.f; // reset timer every second
                    
                    }

                    break; // only chop one tree at a time
                }
            }
            if (!chopping) {
                unit.chopTimer = 0.f; // reset only if no matching tree
            }
        }

        // draw the map
        window.clear();
        window.setView(worldView);
        window.draw(map);

        for (const auto& building : placedBuildings)
            window.draw(building.sprite);

        for (const auto& unit : units) {
            window.draw(unit.sprite);
        }

        for (const Tree& tree : trees) {
            window.draw(tree.sprite);
        }

        if (ghostSprite)
            window.draw(*ghostSprite);

        window.setView(window.getDefaultView());

        // barracks menu
        if (selectedBuildingIndex != -1 && placedBuildings[selectedBuildingIndex].type == BuildingType::Barracks &&
            !ghostSprite) {
            window.draw(trainButton);
            window.draw(trainText);
        }

        window.draw(uiPanel);
        window.draw(barracksIcon);
        window.draw(resourcePanel);
        window.draw(resourceText);
        window.display();
    }
}