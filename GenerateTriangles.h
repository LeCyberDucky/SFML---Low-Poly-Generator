#ifndef GENERATETRIANGLES_H 
#define GENERATETRIANGLES_H 

#include <SFML/Graphics.hpp> 

void getTriangle(sf::ConvexShape& tempTriangle, int_fast32_t pointNum, sf::Vector2f mousePos, sf::RenderWindow& window, int_fast32_t imageSizeX, int_fast32_t imageSizeY);

bool collisionCheck(const sf::ConvexShape& triangle, sf::Vector2f mousePos); 

void setClosePoints(const std::vector<std::pair<int_fast32_t, sf::ConvexShape>>& triangles, sf::ConvexShape& triangle); 

void setAvrgColour(std::vector<std::pair<int_fast32_t, sf::ConvexShape>>& triangles, sf::Sprite& image, const sf::Image& pic, double oldScale); 

void savePicture(std::vector<std::pair<int_fast32_t, sf::ConvexShape>>& triangles, const sf::Image& originalPic, sf::Sprite& workPic, double oldScale, const std::string saveLocation); 

bool pointsFormTriangle(sf::ConvexShape triangle); 

double getX(double y, sf::Vector2f p1, sf::Vector2f p2); 

struct PointWithDistance
{
	sf::Vector2f point; 
	double distance; 
}; 

#endif // !GENERATETRIANGLES_H 