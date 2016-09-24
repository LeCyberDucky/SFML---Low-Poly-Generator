// SFML - Poly test =).cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include "stdafx.h" //#include <SFML/Graphics.hpp> included to solve problems with rect.contains and min/max
#include <SFML/Graphics.hpp> 

#include "GenerateTriangles.h" 

#include <algorithm> 
#include <cstdint> 
#include <fstream> 
#include <string> 
#include <vector> 

int_fast32_t main()
{
	using sf::ConvexShape;				using sf::Mouse; 
	using sf::Vector2f;					using sf::Vector2i; 
	using std::vector;					using std::pair; 
	using std::ifstream;				using std::string; 

	ifstream inFile; 
	inFile.open("Pic Location.txt"); 

	string openLocation; 
	string saveLocation; 
	string iconLocation; 
	string tempString; 
	int_fast16_t lineCount{ 0 };

	while (std::getline(inFile, tempString)) 
	{
		if (lineCount == 0)
		{
			openLocation = tempString;
			++lineCount;
		}

		else if (lineCount == 1) 
		{
			saveLocation = tempString;
			++lineCount; 
		}
		else
			iconLocation = tempString; 
	}

	sf::Image icon; 

	if(!icon.loadFromFile(iconLocation)) 
	{
		return 1;
	}

	sf::ContextSettings aaSettings;
	aaSettings.antialiasingLevel = 16;

	int_fast32_t winSizeX{ 1360 };
	int_fast32_t winSizeY{ 690 };

	sf::RenderWindow window(sf::VideoMode(winSizeX, winSizeY), "Poly test", sf::Style::Default, aaSettings);

	window.setFramerateLimit(60);
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	
	sf::Image originalPic; 
	sf::Texture background; 

	if (!originalPic.loadFromFile(openLocation))
	{
		return 1; 
	}

	if (!background.loadFromImage(originalPic))
	{
		return 1; 
	}

	double startingScale;

	if (originalPic.getSize().x > originalPic.getSize().y)
		startingScale = 800.0 / originalPic.getSize().x;

	else if (originalPic.getSize().x < originalPic.getSize().y)
		startingScale = 450.0 / originalPic.getSize().y; 

	sf::Sprite workPic; 
	workPic.setTexture(background); 
	workPic.setScale(startingScale, startingScale); 

	//Prepare vector of triangles and temporary triangle
	vector<pair<int_fast32_t, ConvexShape>> triangles; 
	ConvexShape tempTriangle; 
	tempTriangle.setPointCount(3); 
	
	int_fast32_t clicks{ 0 };

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{

			switch (event.type)
			{

			case sf::Event::Closed:
				window.close();
				break;


			case sf::Event::MouseButtonPressed:
			{
				Vector2f mousePos = Vector2f::Vector2(sf::Mouse::getPosition(window));

				//Check if user clicked inside a triangle --> do other stuff 
				bool insideTriangle{ false }; 
				bool insidePic{ workPic.getGlobalBounds().contains(mousePos) }; 

				vector<pair<int_fast32_t, ConvexShape>>::size_type triangleAmount{ triangles.size() };

				for (vector<pair<int_fast32_t, ConvexShape>>::iterator i{ triangles.begin() }; i != triangles.begin() + triangleAmount; ++i)
				{
					if (collisionCheck((i->second), mousePos))
					{
						insideTriangle = true;
					}
				}

				if (!insideTriangle && insidePic) 
				{
					if (event.mouseButton.button == Mouse::Left)
					{
						getTriangle(tempTriangle, clicks % 3, mousePos, window, workPic.getGlobalBounds().width, workPic.getGlobalBounds().height);
						++clicks;

						if (clicks % 3 == 0)
						{
							setClosePoints(triangles, tempTriangle); 

							//Don't emplace_back if two points are same or all points build straight line ( = no triangle) 
							bool twoPointsSame{ tempTriangle.getPoint(0) == tempTriangle.getPoint(1) ||
												tempTriangle.getPoint(0) == tempTriangle.getPoint(2) ||
												tempTriangle.getPoint(1) == tempTriangle.getPoint(2) }; 

							bool straightLine{(( tempTriangle.getTransform().transformPoint(tempTriangle.getPoint(0)).x ==
											     tempTriangle.getTransform().transformPoint(tempTriangle.getPoint(1)).x ) 
												&&
												(tempTriangle.getTransform().transformPoint(tempTriangle.getPoint(1)).x ==
											     tempTriangle.getTransform().transformPoint(tempTriangle.getPoint(2)).x)) 
												|| 
											  (( tempTriangle.getTransform().transformPoint(tempTriangle.getPoint(0)).y ==
											     tempTriangle.getTransform().transformPoint(tempTriangle.getPoint(1)).y ) 
												&&
												(tempTriangle.getTransform().transformPoint(tempTriangle.getPoint(1)).y ==
											    tempTriangle.getTransform().transformPoint(tempTriangle.getPoint(2)).y)) };


							if (!twoPointsSame && !straightLine) 
								triangles.emplace_back(0, tempTriangle);
						}
					}
				}

				else if (insideTriangle)
				{
					if (event.mouseButton.button == Mouse::Left)
					{
						//Do stuff to modify triangle (partition further) 
					}

					else if (event.mouseButton.button == Mouse::Right)
					{
						//Delete Triangle 
					}
				}
			}

			case sf::Event::KeyPressed: 
			{
				//Set avg color or undo this
				if (event.key.code == sf::Keyboard::Space)
				{
					//Use bool as switch for setting average color or undo this 
					static bool avgSet{ false }; 

					if (!avgSet)
					{
						setAvrgColour(triangles, workPic, originalPic, workPic.getScale().x); 
						avgSet = !avgSet; 
					}

					else 
					{
						for (vector<pair<int_fast32_t, ConvexShape>>::iterator i = triangles.begin(); i != triangles.end(); ++i)
						{
							i->second.setFillColor(sf::Color::Transparent);
							i->second.setOutlineThickness(0.5);
						}

						avgSet = !avgSet; 
					}
				}

				if (event.key.code == sf::Keyboard::Return) 
				{
					savePicture(triangles, originalPic, workPic, workPic.getScale().x, saveLocation); 
				}
			}

			//Zoom
			case sf::Event::MouseWheelScrolled: 
			{
				double scaleFactor{ 0.01 }; 

				if (event.mouseWheelScroll.delta == -1)
				{
					if (!(workPic.getScale().x < 0.1))
					{
						workPic.scale((startingScale - scaleFactor) / startingScale, (startingScale - scaleFactor) / startingScale);

						for (vector<pair<int_fast32_t, ConvexShape>>::iterator i = triangles.begin(); i != triangles.end(); ++i)
							i->second.scale((startingScale - scaleFactor) / startingScale, (startingScale - scaleFactor) / startingScale);
					}
				}

				if (event.mouseWheelScroll.delta == +1)
				{
					if ((workPic.getScale().x < 1.5))
					{
						workPic.scale(startingScale / (startingScale - scaleFactor), startingScale / (startingScale - scaleFactor));

						for (vector<pair<int_fast32_t, ConvexShape>>::iterator i = triangles.begin(); i != triangles.end(); ++i)
							i->second.scale(startingScale / (startingScale - scaleFactor), startingScale / (startingScale - scaleFactor));
					}
				}
			}

			default:
				break;
			}
		}

		window.clear(sf::Color::Magenta); 
		window.draw(workPic); 
		for (vector<pair<int_fast32_t, ConvexShape>>::iterator i = triangles.begin(); i != triangles.end(); ++i)
		{
			window.draw((i->second));
		}

		window.display();
	}
	return 0;
}