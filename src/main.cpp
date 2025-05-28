#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono> 
#include "Tilemap.hpp"
#include "Building.hpp"
#include "Unit.hpp"

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Tilemap");
    window.setMouseCursorGrabbed(true);

    sf::View worldView({ 800.f, 800.f }, { 300.f, 200.f });

    // define the level with an array of tile indices
    std::vector<int> level(100 * 100, 1);

    // Example: make a diagonal line
    for (int i = 0; i < 100; ++i)
        level[i + i * 100] = 37;

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

    // track units and if they are selected
    std::vector<Unit> units;
    int selectedUnitIndex = -1; // none selected

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

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                sf::Vector2i mouse = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(mouse);

                if (barracksIcon.getGlobalBounds().contains(worldPos)) {
                    selectedBuilding = BuildingType::Barracks;
                    ghostSprite.emplace(Building::tileset);
                    ghostSprite->setTextureRect(getBuildingTileRect(selectedBuilding));
                    ghostSprite->setColor(sf::Color(255, 255, 255, 128));
                }

                if (selectedBuilding != BuildingType::None &&
                    mouse.y < window.getSize().y - 150) {

                    sf::Vector2f worldPos = window.mapPixelToCoords(mouse, worldView);

                    // Snap to tile grid
                    int tileX = static_cast<int>(std::floor(worldPos.x / 16.f));
                    int tileY = static_cast<int>(std::floor(worldPos.y / 16.f));

                    if (!map.isOccupied(tileX, tileY)) {
                        // Place your building here — e.g. spawn building sprite
                        placeBuilding(selectedBuilding, tileX, tileY, placedBuildings);
                        map.markOccupied(tileX, tileY);
                    }

                    ghostSprite.reset();
                    selectedBuilding = BuildingType::None; // reset selection
                }

                for (int i = 0; i < units.size(); ++i) {
                    if (units[i].sprite.getGlobalBounds().contains(worldPos)) {
                        selectedUnitIndex = i;
                        break;
                    }
                }
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                if (selectedUnitIndex >= 0 && selectedUnitIndex < units.size()) {
                    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(mousePixelPos, worldView);
                    sf::Vector2i targetTile = {
                        static_cast<int>(worldPos.x) / tileSize,
                        static_cast<int>(worldPos.y) / tileSize,
                    };

                    units[selectedUnitIndex].targetTile = targetTile;
                }
            }

            auto now = clock::now();
            std::chrono::duration<float> elapsed = now - lastFrameTime;
            float dt = elapsed.count();
            lastFrameTime = now;

            for (auto& building : placedBuildings) {
                if (building.type != BuildingType::Barracks) continue;

                if (!building.buildQueue.empty()) {
                    auto& task = building.buildQueue.front();
                    task.timeRemaining -= dt;
                    if (task.timeRemaining <= 0.f) {
                        // spawn unit
                        Unit newUnit(UnitType::Farmer, Building::tileset, building.tilePosition);
                        units.push_back(newUnit);
                        building.buildQueue.pop();
                    }
                }
            }

            
        }

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

        // draw the map
        window.clear();
        window.setView(worldView);
        window.draw(map);

        for (const auto& building : placedBuildings)
            window.draw(building.sprite);

        if (ghostSprite)
            window.draw(*ghostSprite);

        window.setView(window.getDefaultView());
        window.draw(uiPanel);
        window.draw(barracksIcon);
        window.display();
    }
}