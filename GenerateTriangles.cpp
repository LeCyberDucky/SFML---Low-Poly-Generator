#include "stdafx.h" 

#include "GenerateTriangles.h"

#include <algorithm> 
#include <cstdint> 
#include <numeric> 
#include <SFML/Graphics.hpp> 
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
							tempPoint.x = triNum->second.getTransform().transformPoint(triNum->second.getPoint(triPoNum)).x;
							tempPoint.y = triNum->second.getTransform().transformPoint(triNum->second.getPoint(triPoNum)).y;
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
		int_fast32_t red{ 0 }; 
		int_fast32_t green{ 0 }; 
		int_fast32_t blue{ 0 }; 
		int_fast32_t pixelCount{ 0 }; 

		int_fast32_t maxX = static_cast <int_fast32_t>( (i->second.getGlobalBounds().left + i->second.getGlobalBounds().width) );
		int_fast32_t minX = static_cast <int_fast32_t>(i->second.getGlobalBounds().left);

		int_fast32_t maxY = static_cast <int_fast32_t>( (i->second.getGlobalBounds().top + i->second.getGlobalBounds().height) );
		int_fast32_t minY = static_cast <int_fast32_t>( i->second.getGlobalBounds().top );

		pixel.x = minX; 
		pixel.y = maxY; 

		while (pixel.y >= minY)
		{
			while (pixel.x <= maxX)
			{
				if (collisionCheck(i->second, pixel))
				{
					//Pixels minus 1, because they are stored in a vector (so counting starts at 0 instead of 1. Otherwise, we'd get out of range.)
					red += originalPic.getPixel(pixel.x - 1, pixel.y - 1).r;
					green += originalPic.getPixel(pixel.x - 1, pixel.y - 1).g;
					blue += originalPic.getPixel(pixel.x - 1, pixel.y - 1).b;
					++pixelCount; 
				}
				++pixel.x;
			}

			pixel.x = minX;
			--pixel.y;
		}

		int_fast32_t redAvg{ redAvg = red / pixelCount };
		int_fast32_t greenAvg{ greenAvg = green / pixelCount };
		int_fast32_t blueAvg{ blueAvg = blue / pixelCount };

		i->second.setFillColor(sf::Color(redAvg, greenAvg, blueAvg, 255)); 
		i->second.setOutlineThickness(0); 

		i->second.scale(oldScale, oldScale); 
	}

	workPic.setScale(oldScale, oldScale); 
}

//"Draw the generated triangles onto the original picture" and save this as an image on the harddrive 
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

	sf::Image saveImage; 
	saveImage = savePic.getTexture().copyToImage(); 

	saveImage.saveToFile(saveLocation); 
}
