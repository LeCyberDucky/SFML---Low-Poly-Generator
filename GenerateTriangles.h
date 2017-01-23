#ifndef GENERATETRIANGLES_H 
#define GENERATETRIANGLES_H 

#include <SFML/Graphics.hpp> 

struct Triangle 
{
	sf::ConvexShape shape; 
	sf::Color fillColor; 
	bool colorUpToDate{ 0 };
};

void getTriangle(sf::ConvexShape &tempTriangle, int_fast32_t pointNum, sf::Vector2f mousePos, sf::RenderWindow &window, int_fast32_t imgX, int_fast32_t imgY);

bool collisionCheck(const sf::ConvexShape &triangle, sf::Vector2f mousePos); 

void setClosePoints(const std::vector<std::pair<int_fast32_t, sf::ConvexShape>> &triangles, sf::ConvexShape &triangle); 

bool pointsFormTriangle(sf::ConvexShape triangle); 

double getX(double y, sf::Vector2f p1, sf::Vector2f p2); 

void pointClockSort(sf::ConvexShape &triangle); 

bool clockwiseSort(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f center); 

struct PointWithDistance
{
	sf::Vector2f point; 
	double distance; 
}; 

#endif // !GENERATETRIANGLES_H 