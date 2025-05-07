#include <iostream>
#include <SFML/Graphics.hpp>
#include "Tilemap.hpp"

void clampView(sf::View& view, sf::Vector2u mapSize, sf::Vector2u tileSize, sf::Vector2u windowSize)
{
    sf::Vector2f center = view.getCenter();
    sf::Vector2f halfSize = view.getSize() / 2.f;

    float minX = halfSize.x;
    float maxX = mapSize.x * tileSize.x - halfSize.x;
    float minY = halfSize.y;
    float maxY = mapSize.y * tileSize.y - halfSize.y;

    // Clamp center
    center.x = std::clamp(center.x, minX, maxX);
    center.y = std::clamp(center.y, minY, maxY);

    view.setCenter(center);
}

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Tilemap");
    window.setMouseCursorGrabbed(true);

    sf::View view({ 1600.f, 1600.f }, { 600.f, 400.f });

    // define the level with an array of tile indices
    std::vector<int> level(100 * 100, 0);

    // Example: make a diagonal line
    for (int i = 0; i < 100; ++i)
        level[i + i * 100] = 1;

    // create the tilemap from the level definition
    TileMap map;
    if (!map.load("tileset.png", { 32, 32 }, level.data(), 100, 100))
        return -1;

    float currentZoom = 1.0f;
    const float minZoom = 0.5f;
    const float maxZoom = 2.0f;

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
                        view.zoom(0.9f);
                        currentZoom *= 0.9f;
                    }
                    else if (mouseWheelScrolled->delta < 0 && currentZoom < maxZoom) {
                        view.zoom(1.1f);
                        currentZoom *= 1.1f;
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

        view.move(offset);
        clampView(view, { 100, 100 }, { 32, 32 }, window.getSize());
        window.setView(view);


        

        // draw the map
        window.clear();
        window.draw(map);
        window.display();
    }
}
