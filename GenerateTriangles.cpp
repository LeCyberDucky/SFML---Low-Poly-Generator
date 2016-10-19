#include "stdafx.h" 

#include "GenerateTriangles.h"

#include <algorithm> 
#include <cstdint> 
#include <numeric> 
#include <SFML/Graphics.hpp> 
#include <string> 
#include <vector> 

void getTriangle(sf::ConvexShape& tempTriangle, int_fast32_t pointNum, sf::Vector2f mousePos, sf::RenderWindow& window, int_fast32_t imageSizeX, int_fast32_t imageSizeY)
{
	using sf::Vector2f; 

	Vector2f tempPos; 

	//If Mouseposition close to border, set position of point to border 
	if (mousePos.x <= 8)
		tempPos.x = 0;
	else if (mousePos.x >= imageSizeX - 8)
		tempPos.x = imageSizeX;
	else
		tempPos.x = mousePos.x; 

	if (mousePos.y <= 8)
		tempPos.y = 0;
	else if (mousePos.y >= imageSizeY - 8)
		tempPos.y = imageSizeY; 
	else tempPos.y = mousePos.y; 

	tempTriangle.setPoint(pointNum, tempPos); 
	tempTriangle.setFillColor(sf::Color::Transparent); 
	tempTriangle.setOutlineThickness(-0.5); 
	tempTriangle.setOutlineColor(sf::Color::Cyan); 
} 

//Check if point is within a triangle using barycentric coordinates 
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

//Iterate trough the pixels of each triangle. For each triangle, set it's average colour as fillcolour 
void setAvrgColour(std::vector<std::pair<int_fast32_t, sf::ConvexShape>>& triangles, sf::Sprite& workPic, const sf::Image& originalPic, double oldScale)
{
	sf::Vector2f pixel; 

	double scaleFact{ 1 / oldScale };

	workPic.scale(scaleFact, scaleFact);

	for (std::vector<std::pair<int_fast32_t, sf::ConvexShape>>::iterator i = triangles.begin(); i != triangles.end(); ++i)
	{
		i->second.scale(scaleFact, scaleFact); 
	}

	for (std::vector<std::pair<int_fast32_t, sf::ConvexShape>>::iterator i = triangles.begin(); i != triangles.end(); ++i)
	{
		int_fast64_t red{ 0 }; 
		int_fast64_t green{ 0 }; 
		int_fast64_t blue{ 0 }; 
		int_fast64_t pixelCount{ 0 }; 

		double maxX{ 0 }; 
		double minX{ 0 }; 
		double maxY{ 0 }; 
		double minY{ 0 }; 

		/*Experimental code to speed up process of iterating trough pixels 
		 *Seems to be working and to be way faster than the old aproach (yay) where 
		 *we'd iterate trough every pixel of the global bounds rectangel of the triangel 
		 *and check if that pixel is inside the triangle using collisionCheck*/

		sf::Vector2f highestPoint; 
		sf::Vector2f lowestPoint; 
		sf::Vector2f middlePoint; 

		if (i->second.getPoint(0).y < i->second.getPoint(1).y &&
			i->second.getPoint(0).y < i->second.getPoint(2).y)
		{
			highestPoint = i->second.getTransform().transformPoint(i->second.getPoint(0)); 

			if (i->second.getPoint(1).y < i->second.getPoint(2).y) 
			{
				middlePoint = i->second.getTransform().transformPoint(i->second.getPoint(1)); 
				lowestPoint = i->second.getTransform().transformPoint(i->second.getPoint(2)); 
			}

			else 
			{
				middlePoint = i->second.getTransform().transformPoint(i->second.getPoint(2)); 
				lowestPoint = i->second.getTransform().transformPoint(i->second.getPoint(1)); 
			}
		}

		else if (i->second.getPoint(1).y < i->second.getPoint(0).y &&
			i->second.getPoint(1).y < i->second.getPoint(2).y)
		{
			highestPoint = i->second.getTransform().transformPoint(i->second.getPoint(1)); 

			if (i->second.getPoint(0).y < i->second.getPoint(2).y)
			{
				middlePoint = i->second.getTransform().transformPoint(i->second.getPoint(0)); 
				lowestPoint = i->second.getTransform().transformPoint(i->second.getPoint(2)); 
			}

			else
			{
				middlePoint = i->second.getTransform().transformPoint(i->second.getPoint(2)); 
				lowestPoint = i->second.getTransform().transformPoint(i->second.getPoint(0)); 
			}
		}

		else
		{
			highestPoint = i->second.getTransform().transformPoint(i->second.getPoint(2)); 

			if (i->second.getPoint(0).y < i->second.getPoint(1).y)
			{
				middlePoint = i->second.getTransform().transformPoint(i->second.getPoint(0)); 
				lowestPoint = i->second.getTransform().transformPoint(i->second.getPoint(1)); 
			}

			else
			{
				middlePoint = i->second.getTransform().transformPoint(i->second.getPoint(1)); 
				lowestPoint = i->second.getTransform().transformPoint(i->second.getPoint(0)); 
			}
		}

		pixel.y = lowestPoint.y;

		//MiddlePoint is on right side. -> long line is left -> iterate left to right 
		//if (middlePoint.x > highestPoint.x) 
		if (middlePoint.x > getX(middlePoint.y, lowestPoint, highestPoint)) 
		{

			while (pixel.y > highestPoint.y)
			{
				pixel.x = getX(pixel.y, lowestPoint, highestPoint); 

				//We don't want decimal pixels =/ 
				int_fast32_t rounder = static_cast <int_fast32_t>(pixel.x);
				if (pixel.x - 0.5 > rounder)
					pixel.x = rounder + 1;
				else
					pixel.x = rounder;

				if (pixel.y > middlePoint.y) 
				{
					maxX = getX(pixel.y, lowestPoint, middlePoint); 

					rounder = static_cast <int_fast32_t>(maxX); 
					if (maxX - 0.5 > rounder)
						maxX = rounder + 1; 
					else
						maxX = rounder; 
				}

				else 
				{
					maxX = getX(pixel.y, middlePoint, highestPoint); 

					rounder = static_cast <int_fast32_t>(maxX);
					if (maxX - 0.5 > rounder)
						maxX = rounder + 1;
					else
						maxX = rounder; 
				}

				while (pixel.x <= maxX) 
				{
					red += originalPic.getPixel(pixel.x - 1, pixel.y - 1).r;
					green += originalPic.getPixel(pixel.x - 1, pixel.y - 1).g;
					blue += originalPic.getPixel(pixel.x - 1, pixel.y - 1).b;
					++pixelCount;

					++pixel.x;
				}

				--pixel.y;
			}
		}
		
		else 
		{
			while (pixel.y > highestPoint.y)
			{
				/*pixel.x = getX(pixel.y, lowestPoint, highestPoint); 

				int_fast32_t rounder = static_cast <int_fast32_t>(pixel.x);
				if (pixel.x - 0.5 > rounder)
					pixel.x = rounder + 1;
				else
					pixel.x = rounder; */ 

				maxX = getX(pixel.y, lowestPoint, highestPoint); 

				/*if (pixel.y > middlePoint.y) 
					minX = getX(pixel.y, lowestPoint, middlePoint); 

				else
				{
					minX = getX(pixel.y, middlePoint, highestPoint); 

					rounder = static_cast <int_fast32_t>(minX);
					if (minX - 0.5 > rounder)
						minX = rounder + 1;
					else
						minX = rounder;
				}*/ 

				if (pixel.y > middlePoint.y)
					pixel.x = getX(pixel.y, lowestPoint, middlePoint);

				else
					pixel.x = getX(pixel.y, middlePoint, highestPoint); 

				while (pixel.x <= maxX)
				{
					red += originalPic.getPixel(pixel.x, pixel.y).r;
					green += originalPic.getPixel(pixel.x, pixel.y).g;
					blue += originalPic.getPixel(pixel.x, pixel.y).b;

					++pixelCount;

					++pixel.x; 
				}

				--pixel.y;
			}
		}

		//No division if the pixelCount is 0 
		if (pixelCount)
		{
			int_fast32_t redAvg{ 0 };
			int_fast32_t greenAvg{ 0 };
			int_fast32_t blueAvg{ 0 };

			redAvg = red / pixelCount;
			greenAvg = green / pixelCount;
			blueAvg = blue / pixelCount;

			i->second.setFillColor(sf::Color(redAvg, greenAvg, blueAvg, 255));
		}
		i->second.setOutlineThickness(0); 

		i->second.scale(oldScale, oldScale); 
	}

	workPic.setScale(oldScale, oldScale); 
}

//"Draw the generated triangles onto the original picture" and save this as an image to the harddrive 
void savePicture(std::vector<std::pair<int_fast32_t, sf::ConvexShape>>& triangles, const sf::Image& originalPic, sf::Sprite& workPic, double oldScale, const std::string saveLocation)
{
	sf::RenderTexture savePic;
	if (!savePic.create(originalPic.getSize().x, originalPic.getSize().y)) { /*Do stuff*/ } 

	double scaleFact{ 1 / oldScale };
	workPic.scale(scaleFact, scaleFact);
	savePic.draw(workPic); 
	workPic.scale(oldScale, oldScale); 

	for (std::vector<std::pair<int_fast32_t, sf::ConvexShape>>::iterator i = triangles.begin(); i != triangles.end(); ++i)
	{
		i->second.scale(scaleFact, scaleFact); 
		savePic.draw(i->second); 
		i->second.scale(oldScale, oldScale);  
	}
	
	savePic.display(); 

	savePic.getTexture().copyToImage().saveToFile(saveLocation); 
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