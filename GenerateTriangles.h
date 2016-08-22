#ifndef GENERATETRIANGLES_H 
#define GENERATETRIANGLES_H 

#include <SFML/Graphics.hpp> 

void getTriangle(sf::ConvexShape& tempTriangle, int_fast32_t pointNum, sf::Vector2f mousePos, sf::RenderWindow& window, int_fast32_t imageSizeX, int_fast32_t imageSizeY);

bool collisionCheck(const sf::ConvexShape& triangle, sf::Vector2f mousePos); 

void setClosePoints(const std::vector<std::pair<int_fast32_t, sf::ConvexShape>>& triangles, sf::ConvexShape& triangle); 

void setAvrgColour(std::vector<std::pair<int_fast32_t, sf::ConvexShape>>& triangles, sf::Sprite& image, const sf::Texture background, const sf::Image& pic, double oldScale);

struct PointWithDistance
{
	sf::Vector2f point; 
	double distance; 
}; 

#endif // !GENERATETRIANGLES_H 