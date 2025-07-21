#pragma once
#include <SFML/Graphics.hpp>

class Tilemap : public sf::Drawable, public sf::Transformable
{
public:
    bool load(const std::filesystem::path& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height);
    bool isOccupied(int x, int y) const;
    void markOccupied(int x, int y);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::VertexArray m_vertices;
    sf::Texture     m_tileset;
    bool occupied[100][100] = {};
};
 
void clampView(sf::View& view, sf::Vector2u mapSize, sf::Vector2u tileSize, sf::Vector2u windowSize);