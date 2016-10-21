#include "stdafx.h" 

#include "GenerateTriangles.h"

#include <algorithm> 
#include <cstdint> 
#include <numeric> 
#include <SFML/Graphics.hpp> 
#include <string> 
#include <vector> 

void getTriangle(sf::ConvexShape& tempTriangle, int_fast32_t pointNum, sf::Vector2f mousePos, sf::RenderWindow& window, int_fast32_t imgX, int_fast32_t imgY)
{
	using sf::Vector2f; 

	Vector2f tempPos; 

	//If Mouseposition close to border, set position of point to border 
	if (mousePos.x <= 8)
		tempPos.x = 0;
	else if (mousePos.x >= imgX - 8)
		tempPos.x = imgX;
	else
		tempPos.x = mousePos.x; 

	if (mousePos.y <= 8)
		tempPos.y = 0;
	else if (mousePos.y >= imgY - 8)
		tempPos.y = imgY; 
	else tempPos.y = mousePos.y; 

	tempTriangle.setPoint(pointNum, tempPos); 
	tempTriangle.setFillColor(sf::Color::Transparent); 
	tempTriangle.setOutlineThickness(-0.5); 
	tempTriangle.setOutlineColor(sf::Color::Cyan); 
} 

/*Check if point is within a triangle using barycentric coordinates 
 *http://stackoverflow.com/questions/13300904/determine-whether-point-lies-inside-triangle 
 */
bool collisionCheck(const sf::ConvexShape& triangle, sf::Vector2f checkPoint)
{
	double cpX{ checkPoint.x };
	double cpY{ checkPoint.y };

	double p1X{ triangle.getTransform().transformPoint(triangle.getPoint(0)).x };
	double p1Y{ triangle.getTransform().transformPoint(triangle.getPoint(0)).y };

	double p2X{ triangle.getTransform().transformPoint(triangle.getPoint(1)).x };
	double p2Y{ triangle.getTransform().transformPoint(triangle.getPoint(1)).y };

	double p3X{ triangle.getTransform().transformPoint(triangle.getPoint(2)).x };
	double p3Y{ triangle.getTransform().transformPoint(triangle.getPoint(2)).y };

	double alpha{ 0 };
	double beta{ 0 };
	double gamma{ 0 };

	alpha = ((p2Y - p3Y)*(cpX - p3X) + (p3X - p2X)*(cpY - p3Y)) /
		((p2Y - p3Y)*(p1X - p3X) + (p3X - p2X)*(p1Y - p3Y));

	beta = ((p3Y - p1Y)*(cpX - p3X) + (p1X - p3X)*(cpY - p3Y)) /
		((p2Y - p3Y)*(p1X - p3X) + (p3X - p2X)*(p1Y - p3Y));

	gamma = 1.0 - alpha - beta;

	return (alpha > 0 && beta > 0 && gamma > 0);
}

//If any point of the newest triangle is close to the point of another triangle, set the point of the new triangle to the existing one
void setClosePoints(const std::vector<std::pair<int_fast32_t, sf::ConvexShape>>& triangles, sf::ConvexShape& triangle)
{
	 for (int_fast32_t pointNum{ 0 }; pointNum != 3; ++pointNum)
	{
		for (std::vector<std::pair<int_fast32_t, sf::ConvexShape>>::const_iterator triNum = triangles.begin(); triNum != triangles.end(); ++triNum)
		{
			for (int_fast32_t triPoNum{ 0 }; triPoNum != 3; ++triPoNum)
			{
				sf::Vector2f tempPoint{ triangle.getPoint(pointNum) };

				if (((triNum->second.getTransform().transformPoint(triNum->second.getPoint(triPoNum)).x - 20) <= triangle.getPoint(pointNum).x) && 
					 (triangle.getPoint(pointNum).x <= (triNum->second.getTransform().transformPoint(triNum->second.getPoint(triPoNum)).x + 20)))
					//----------------------------------------------------------------------------------------------------------------------------
					if (((triNum->second.getTransform().transformPoint(triNum->second.getPoint(triPoNum)).y - 20) <= triangle.getPoint(pointNum).y) &&
						(triangle.getPoint(pointNum).y <= (triNum->second.getTransform().transformPoint(triNum->second.getPoint(triPoNum)).y + 20)))
						{
							tempPoint = triNum->second.getTransform().transformPoint(triNum->second.getPoint(triPoNum)); 
						}

				triangle.setPoint(pointNum, tempPoint);
			}
		}
	}
}

//Returns true, if forming a triangle is possible with the given points
bool pointsFormTriangle(sf::ConvexShape triangle)
{
	bool twoPointsSame{ triangle.getPoint(0) == triangle.getPoint(1) ||
		triangle.getPoint(0) == triangle.getPoint(2) ||
		triangle.getPoint(1) == triangle.getPoint(2) };

	bool straightLine{ ((triangle.getTransform().transformPoint(triangle.getPoint(0)).x ==
		triangle.getTransform().transformPoint(triangle.getPoint(1)).x)
		&&
		(triangle.getTransform().transformPoint(triangle.getPoint(1)).x ==
			triangle.getTransform().transformPoint(triangle.getPoint(2)).x))
		||
		((triangle.getTransform().transformPoint(triangle.getPoint(0)).y ==
			triangle.getTransform().transformPoint(triangle.getPoint(1)).y)
			&&
			(triangle.getTransform().transformPoint(triangle.getPoint(1)).y ==
				triangle.getTransform().transformPoint(triangle.getPoint(2)).y)) }; 

	return !(twoPointsSame || straightLine); 
}

//Returns the corresponding x-value given the y-value and two points that form a line
double getX(double y, sf::Vector2f p1, sf::Vector2f p2) 
{
	/*
	y1=a*x1+b
	b=y1-a*x1

	y2=a*x2+b
	b=y2-a*x2

	y1-x1*a=y2-x2*a

	a=(y2-y1)/(x2-x1)

	b=y1-x1*((y2-y1)/(x2-x1))

	y=a*x+b 

	x=((x2-x1)*y+x1*y2-y1*x2)/(y2-y1)

	y = pixel.y
	x1 = lowestPoint.x		y1 = lowestPoint.y
	x2 = highestPoint.x		y2 = h
	*/

	return ((p2.x - p1.x) * y + p1.x * p2.y - p1.y * p2.x) / (p2.y - p1.y); 
}

/*Sorts the points of a triangle in clockwise order. The points need to be in 
either clockwise or counter-clockwise order. Otherwise the drawing gets messed up*/
void pointClockSort(sf::ConvexShape& triangle) 
{
	using std::vector; 

	vector<sf::Vector2f> points; 

	for (int_fast32_t i = 0; i != 3; ++i)
		points.emplace_back(triangle.getTransform().transformPoint(triangle.getPoint(i)));

	//Generate centerpoint of triangle. http://www.mathopenref.com/coordcentroid.html 
	//centerX = (Ax + Bx + Cx) / 3.0			centerY = (Ay + By + Cy) / 3.0 

	sf::Vector2f center; 

	center.x = ((points[0].x + points[1].x + points[2].x) / 3.0); 
	center.y = ((points[0].y + points[1].y + points[2].y) / 3.0 ); 

	//Sort the vector of points clockwise http://stackoverflow.com/questions/6989100/sort-points-in-clockwise-order
	std::sort(points.begin(), points.end(), [center](sf::Vector2f p1, sf::Vector2f p2)  
	{
		return clockwiseSort(p1, p2, center); 
	}); 

	triangle.getTransform().transformPoint(triangle.getPoint(0)) = points[0]; 
	triangle.getTransform().transformPoint(triangle.getPoint(1)) = points[1];
	triangle.getTransform().transformPoint(triangle.getPoint(2)) = points[2];
}

bool clockwiseSort(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f center) 
{
	if (p1.x - center.x >= 0 && p2.x - center.x < 0)
		return true; 
	if (p1.x - center.x < 0 && p2.x - center.x >= 0)
		return false; 
	if (p1.x - center.x == 0 && p2.x - center.x == 0) 
	{
		if (p1.y - center.y >= 0 || p2.y - center.y >= 0)
			return p1.y > p2.y; 
		return p2.y > p1.y; 
	}

	//compute the cross product of vectors (center -> a) x (center -> b) 
	int_fast32_t det = (p1.x - center.x) * (p2.y - center.y) - (p2.x - center.x) * (p1.y - center.y); 

	if (det < 0)
		return true; 
	if (det > 0)
		return false; 

	//points a and b are on the same line from the center 
	//check which point is closer to the center 
	int_fast32_t d1 = (p1.x - center.x) * (p1.x - center.x) + (p1.y - center.y) * (p1.y - center.y); 
	int_fast32_t d2 = (p2.x - center.x) * (p2.x - center.x) + (p2.y - center.y) * (p2.y - center.y); 

	return d1 > d2; 
}