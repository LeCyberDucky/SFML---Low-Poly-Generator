#include "stdafx.h" 

#include "Manipulation.h" 
#include "GenerateTriangles.h"

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
				maxX = getX(pixel.y, lowestPoint, highestPoint);

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
