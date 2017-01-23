#include "stdafx.h" 

#include <filesystem> 
#include <fstream> 
#include <string>

/*Scales a copy of all triangles to fit the original picture.
*Then saves the coordinates of every triangle to a file*/
void saveTriangleCoords(std::vector<std::pair<int_fast32_t, sf::ConvexShape>> triangles, double oldScale)
{
	namespace fs = std::experimental::filesystem; 

	std::ofstream saveFile;
	saveFile.open("saveTriangleCoords.txt");
	
	double scaleFact{ 1 / oldScale };

	for (std::vector<std::pair<int_fast32_t, sf::ConvexShape>>::iterator i = triangles.begin(); i != triangles.end(); ++i)
	{
		i->second.scale(scaleFact, scaleFact);
	}
}

void errorWindow(const std::string &errMessage) 
{ }