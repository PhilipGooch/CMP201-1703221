#include "Pathfinding.h"

int main()
{
	const int w = 12;
	const int h = 12;
	const int squareSize = 50;
	const int indent = 0;
	const int windowWidth = indent * 2 + w * (squareSize + 1);
	const int windowHeight = indent * 2 + h * (squareSize + 1);

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Pathfinding");

	//window.setPosition(sf::Vector2i(763, 3));
	window.setPosition(sf::Vector2i(365, 3));

	Pathfinding pathfinding(&window);

	window.setFramerateLimit(60);

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
			}
		}
	}
	
	return 0;
}