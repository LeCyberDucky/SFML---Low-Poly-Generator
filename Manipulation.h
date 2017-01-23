#ifndef MANIPULATION_H 
#define MANIPULATION_H 

#include <SFML/Graphics.hpp> 

void setAvrgColour(std::vector<std::pair<int_fast32_t, sf::ConvexShape>> &triangles, sf::Sprite &image, const sf::Image &pic, double oldScale);

void savePicture(std::vector<std::pair<int_fast32_t, sf::ConvexShape>> &triangles, const sf::Image &originalPic, sf::Sprite &workPic, double oldScale, const std::string saveLocation);

#endif 