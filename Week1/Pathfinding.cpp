#include "Pathfinding.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//I have only commented the rendering code in this project. The comments for the rest of the code are in my testing project.//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Pathfinding::Pathfinding(sf::RenderWindow* hwnd)
{
	window = hwnd;

	//Populating my vector of verticies for parent indicator lines when rendering.
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			sf::Vertex line[2];
			lines.push_back(line);
		}
	}

	//Loading font for rendering.
	if (!font.loadFromFile("font/arial.ttf")) {
		std::cout << "Error reading file." << std::endl;
	}
	
	renderSetup();

	delay = 200;
	//algorithm = Dijkstra;
	algorithm = AStar;
	dataStructure = vector;		
	//dataStructure = priority_queue;	//Remember to switch to appropriate functor with priority_queue!
	rendering = true;
	diagonals = true;

	if (rendering) {
		generateMap();
		if (algorithm == AStar) {
			if (dataStructure == vector) {
				aStarVectorRendering();
			}
			else if (dataStructure == priorityQueue) {
				aStarPriorityQueueRendering();
			}
		}
		if (algorithm == Dijkstra) {
			if (dataStructure == vector) {
				dijkstraVectorRendering();
			}
			else if (dataStructure == priorityQueue) {
				dijkstraPriorityQueueRendering();
			}
		}
		backTracingRendering();
	}
	else {
		generateMap();
		if (algorithm == AStar) {
			if (dataStructure == vector) {
				aStarVector();
			}
			else if (dataStructure == priorityQueue) {
				aStarPriorityQueue();
			}
		}
		if (algorithm == Dijkstra) {
			if (dataStructure == vector) {
				dijkstraVector();
			}
			else if (dataStructure == priorityQueue) {
				dijkstraPriorityQueue();
			}
		}
		backTracing();
		render();
	}
	printMap();

}

Pathfinding::~Pathfinding()
{
	for (Node* node : graph) {		//Do i need to do this?
		delete node;
	}
}

void Pathfinding::generateMap()
{
	//Reading file and storing data.
	std::ifstream file;
	std::string line;
	file.open("map.txt");
	if (file.is_open())
	{
		int row = 0;
		while (getline(file, line))
		{
			fileMap[row++] = line;
		}
		file.close();
	}
	else std::cout << "File not found.";

	//Making sure the data structures are empty.
	graph.clear();
	openVector.clear();
	closedVector.clear();
	while (!openPriorityQueue.empty()) {
		openPriorityQueue.pop();
	}
	while (!closedPriorityQueue.empty()) {
		closedPriorityQueue.pop();
	}

	//Using data to populate graph.
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (fileMap[i][j] == '#') {
				graph.push_back(nullptr);
			}
			else {
				graph.push_back(new Node(i, j));
				if (fileMap[i][j] == 'o') {
					startNode = graph[graph.size() - 1];
				}
				else if (fileMap[i][j] == 'x') {
					endNode = graph[graph.size() - 1];
				}
			}
		}
	}

	//  O--O--O--O--O--O    .  .  .  .  .  .
	//  |  |  |  |  |  | 
	//  O--O--O--O--O--O	.  .  .  .  .  .
	//           |  |  |
	//			 O--O--O    #  #  #  .  .  .
	//           |  |  |
	//  O--O--O--O--O--O    .  .  .  .  .  .
	//  |  |  |     |  |
	//  O--O--O     O--O    .  .  .  #  .  .
	//  |  |  |     |  |
	//  O--O--O--O--O--O    .  .  .  .  .  .				//I could sort the connections into order to get a nicer animation...

	//Connecting nodes.
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (graph[i * w + j] != nullptr) {											//If there is a node here:	
				//Top
				if (i > 0 && graph[(i - 1) * w + j] != nullptr) {						//If not in top row and there is a node above it:
					connect(graph[i * w + j], graph[(i - 1) * w + j]);
				}
				//Top-Left
				if (diagonals) {
					if (i > 0 && j > 0 && graph[(i - 1) * w + j - 1] != nullptr) {			//If not in top row, not in far left column and there is a node top-left of it:
						connect(graph[i * w + j], graph[(i - 1) * w + j - 1]);
					}
				}
				//Left
				if (j > 0 && graph[i * w + j - 1] != nullptr) {							//If not in far left column and there is a node left of it:
					connect(graph[i * w + j], graph[i * w + j - 1]);
				}
				//Bottom-Left
				if (diagonals) {
					if (i < h - 1 && j > 0 && graph[(i + 1) * w + j - 1] != nullptr) {		//If not in bottom row, not in far left column and there is a node bottom-left of it:
						connect(graph[i * w + j], graph[(i + 1) * w + j - 1]);
					}
				}
				//Bottom
				if (i < h - 1 && graph[(i + 1) * w + j] != nullptr) {					//If not in bottom row and there is a node below it:
					connect(graph[i * w + j], graph[(i + 1) * w + j]);
				}
				//Bottom-Right
				if (diagonals) {
					if (i < h - 1 && j < w - 1 && graph[(i + 1) * w + j + 1] != nullptr) {	//If not in bottom row, not in far right column and there is a node bottom-right of it:
						connect(graph[i * w + j], graph[(i + 1) * w + j + 1]);
					}
				}
				//Right
				if (j < w - 1 && graph[i * w + j + 1] != nullptr) {						//If not in far right column and there is a node right of it:
					connect(graph[i * w + j], graph[i * w + j + 1]);
				}
				//Top-Right
				if (diagonals) {
					if (i > 0 && j < w - 1 && graph[(i - 1) * w + j + 1] != nullptr) {		//If not in top row, not in far right column and there is a node top-right of it:
						connect(graph[i * w + j], graph[(i - 1) * w + j + 1]);
					}
				}
			}
		}
	}
}

//For comments on the algorithms, see Pathfinding Testing project.

void Pathfinding::aStarVector()
{
	startNode->g = 0;
	currentNode = startNode;
	currentNode->open = true;
	openVector.push_back(currentNode);	
	for (auto node : graph) {
		if (node != nullptr) {
			if (node != startNode && node != endNode) {
				calculateH(node);
			}
		}
	}
	while (true) {
		currentNode->open = false;
		removeFromVector(openVector, currentNode);		
		currentNode->closed = true;
		closedVector.push_back(currentNode);
		for (auto node : currentNode->neighbors) {
			if (node->closed) {		
				continue;
			}
			else if (node->open) {											
				if (currentNode->g + calculateMovementValue(currentNode, node) < node->g) {	
					node->parent = currentNode;
					calculateG(node, calculateMovementValue(currentNode, node));
					calculateF(node);
				}
				continue;
			}
			node->parent = currentNode;
			calculateG(node, calculateMovementValue(currentNode, node));
			calculateF(node);
			node->open = true;
			openVector.push_back(node);
			if (node == endNode) {
				node->open = false;
				node->closed = true;
				return;
			}
		}
		currentNode = lowestFNodeInVector(openVector);
	}
}

void Pathfinding::aStarPriorityQueue()		
{
	startNode->g = 0;
	currentNode = startNode;
	currentNode->open = true;
	openPriorityQueue.push(currentNode);
	for (auto node : graph) {
		if (node != nullptr) {
			if (node != startNode && node != endNode) {
				calculateH(node);
			}
		}
	}
	while (true) {
		currentNode->open = false;
		openPriorityQueue.pop();
		currentNode->closed = true;
		closedPriorityQueue.push(currentNode);
		for (auto node : currentNode->neighbors) {
			if (node->closed) {
				continue;
			}
			else if (node->open) {
				if (currentNode->g + calculateMovementValue(currentNode, node) < node->g) {		
					node->parent = currentNode;
					calculateG(node, calculateMovementValue(currentNode, node));
					calculateF(node);
				}
				continue;
			}
			node->parent = currentNode;
			calculateG(node, calculateMovementValue(currentNode, node));
			calculateF(node);
			node->open = true;
			openPriorityQueue.push(node);
			if (node == endNode) {
				node->open = false;
				node->closed = true;
				return;
			}
		}
		currentNode = openPriorityQueue.top();
	}
}

void Pathfinding::dijkstraVector()
{
	startNode->g = 0;
	currentNode = startNode;
	currentNode->open = true;
	openVector.push_back(currentNode);
	int movementValue;
	while (true) {
		if (currentNode == nullptr) {	
			break;
		}
		currentNode->open = false;
		removeFromVector(openVector, currentNode);
		currentNode->closed = true;
		closedVector.push_back(currentNode);
		for (auto node : currentNode->neighbors) {
			if (node->closed == true) {
				continue;
			}
			else if (node->open == true) {
				movementValue = calculateMovementValue(currentNode, node);
				if (currentNode->g + movementValue < node->g) {
					node->parent = currentNode;
					calculateG(node, calculateMovementValue(currentNode, node));
				}
				continue;
			}
			node->parent = currentNode;
			calculateG(node, calculateMovementValue(currentNode, node));
			node->open = true;
			openVector.push_back(node);
		}
		currentNode = lowestFNodeInVector(openVector);
	}
}

void Pathfinding::dijkstraPriorityQueue()
{
	startNode->g = 0;
	currentNode = startNode;
	currentNode->open = true;
	openPriorityQueue.push(currentNode);
	int movementValue;
	while (true) {
		if (currentNode == nullptr) {	
			break;
		}
		currentNode->open = false;
		openPriorityQueue.pop();
		currentNode->closed = true;
		closedPriorityQueue.push(currentNode);
		for (auto node : currentNode->neighbors) {
			if (node->closed == true) {
				continue;
			}
			else if (node->open == true) {
				movementValue = calculateMovementValue(currentNode, node);
				if (currentNode->g + movementValue < node->g) {
					node->parent = currentNode;
					calculateG(node, calculateMovementValue(currentNode, node));
				}
				continue;
			}
			node->parent = currentNode;
			calculateG(node, calculateMovementValue(currentNode, node));
			node->open = true;
			openPriorityQueue.push(node);
		}
		if (openPriorityQueue.empty()) {
			currentNode = nullptr;
		}
		else {
			currentNode = openPriorityQueue.top();
		}
	}
}

void Pathfinding::backTracing()
{
	currentNode = endNode;
	do {
		currentNode->path = true;
		currentNode = currentNode->parent;
	} while (currentNode != startNode);
}

void Pathfinding::connect(Node* a, Node* b)
{
	for (auto n : a->neighbors) {
		if (n == b) {
			return;
		}
	}
	a->neighbors.push_back(b);
	b->neighbors.push_back(a);
}

void Pathfinding::calculateF(Node* node)
{
	node->f = node->g + node->h;
}

void Pathfinding::calculateG(Node* node, int movementValue)
{
	node->g = node->parent->g + movementValue;
}

void Pathfinding::calculateH(Node* node)
{
	node->h = (abs(endNode->j - node->j) + abs(endNode->i - node->i)) * 10;							//Manhattan distance.
	//node->h = (int)(sqrt(pow(node->j - endNode->j, 2) + pow(node->i - endNode->i, 2)) * 10);		//Euclidean distance.
}

int Pathfinding::calculateMovementValue(Node* a, Node* b)		
{
	if ((b->i == a->i && (b->j == a->j + 1 || b->j == a->j - 1)) || (b->j == a->j && (b->i == a->i + 1 || b->i == a->i - 1))) {
		return 10;
	}
	return 14;
}
 
void Pathfinding::removeFromVector(std::vector<Node*>& vector, Node* node)
{
	for (std::vector<Node*>::iterator it = vector.begin(); it != vector.end(); ++it) {
		if (node == *it) {
			vector.erase(it);
			return;
		}
	}
}

Node* Pathfinding::lowestFNodeInVector(std::vector<Node*>& vector)
{
	int lowestF = INT32_MAX;
	Node* lowestFNode = nullptr;								
	for (int i = 0; i < openVector.size(); i++) {
		if (openVector[i] != nullptr) {
			if (openVector[i]->f < lowestF) {
				lowestF = openVector[i]->f;
				lowestFNode = openVector[i];
			}
		}
	}
	return lowestFNode;
}

//Setting up sfml shape objects perameters.
void Pathfinding::renderSetup()
{
	//Neighbor node rectangle shape.
	neighborNodeIndicator.setSize(sf::Vector2f(squareSize, squareSize));
	neighborNodeIndicator.setOutlineColor(sf::Color(150, 150, 150));
	neighborNodeIndicator.setOutlineThickness(-3);
	neighborNodeIndicator.setFillColor(sf::Color(0, 0, 0, 0));
	//Current node rectangle shape.
	currentNodeIndicator.setSize(sf::Vector2f(squareSize, squareSize));
	currentNodeIndicator.setOutlineColor(sf::Color::Black);
	currentNodeIndicator.setOutlineThickness(-3);
	currentNodeIndicator.setFillColor(sf::Color(0, 0, 0, 0));
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			//Rectangle shapes for each cell in the grid.
			squares[i][j].setSize(sf::Vector2f(squareSize, squareSize));
			squares[i][j].setPosition(indent + j * (squareSize + 1), indent + i * (squareSize + 1));
			//Text objects for F, G and H values.
			textF[i][j].setFont(font);
			textF[i][j].setCharacterSize(11);
			textF[i][j].setFillColor(sf::Color::Black);
			textG[i][j].setFont(font);
			textG[i][j].setCharacterSize(11);
			textG[i][j].setFillColor(sf::Color::Black);
			textH[i][j].setFont(font);
			textH[i][j].setCharacterSize(11);
			textH[i][j].setFillColor(sf::Color::Black);
			//Circles for open/closed set indicators.
			circles[i][j].setPointCount(90);
			circles[i][j].setOutlineColor(sf::Color::Black);
			circles[i][j].setOutlineThickness(2);
			circles[i][j].setRadius(squareSize * 0.06);
			circles[i][j].setOrigin(squareSize * 0.06, squareSize * 0.06);
			circles[i][j].setPosition(indent + j * (squareSize + 1) + squareSize * 0.5, indent + i * (squareSize + 1) + squareSize * 0.5);
		}
	}
}

void Pathfinding::render()
{
	//Clearing the window.
	window->clear(sf::Color::White);
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			//Rectangle shapes.
			//Wall
			if (graph[i * w + j] == nullptr) {
				squares[i][j].setFillColor(sf::Color::Black);
			}
			else {
				//Start
				if (graph[i * w + j] == startNode) {
					squares[i][j].setFillColor(sf::Color(204, 255, 204));
				}
				//End
				else if (graph[i * w + j] == endNode) {
					squares[i][j].setFillColor(sf::Color(255, 204, 204));
				}
				//Path
				else if (graph[i * w + j]->path) {
					squares[i][j].setFillColor(sf::Color(255, 255, 204));
				}
				//Empty
				else {
					squares[i][j].setFillColor(sf::Color(200, 200, 200));
				}
			}
			window->draw(squares[i][j]);
			//If not a wall:
			if (graph[i * w + j] != nullptr) {
				//Text.
				//If it is not the start or end node:
				if (graph[i * w + j] != startNode && graph[i * w + j] != endNode) {
					//If the node's F value has been calculated:
					if (graph[i * w + j]->f != -1) {
						//Set text object's string to it's F value.
						textF[i][j].setString(std::to_string(graph[i * w + j]->f));
						textF[i][j].setPosition(indent + j * (squareSize + 1) + squareSize * 0.05, indent + i * (squareSize + 1) + squareSize * 0.0);
						window->draw(textF[i][j]);
					}
					//If the node's G value has been calculated:
					if (graph[i * w + j]->g != -1) {
						//Set text object's string to it's G value.
						textG[i][j].setString(std::to_string(graph[i * w + j]->g));
						textG[i][j].setPosition(indent + j * (squareSize + 1) + squareSize * 0.05, indent + i * (squareSize + 1) + squareSize * 0.7);
						window->draw(textG[i][j]);
					}
					//If the node's H value has been calculated:
					if (graph[i * w + j]->h != -1) {
						//Set text object's string to it's H value.
						textH[i][j].setString(std::to_string(graph[i * w + j]->h));
						//Set its position, offset for how many digits it has as it is on the right.
						if (graph[i * w + j]->h >= 1000) {
							textH[i][j].setPosition(indent + j * (squareSize + 1) + squareSize * 0.48, indent + i * (squareSize + 1) + squareSize * 0.7);
						}
						else if (graph[i * w + j]->h >= 100) {
							textH[i][j].setPosition(indent + j * (squareSize + 1) + squareSize * 0.59, indent + i * (squareSize + 1) + squareSize * 0.7);
						}
						else if (graph[i * w + j]->h >= 10) {
							textH[i][j].setPosition(indent + j * (squareSize + 1) + squareSize * 0.70, indent + i * (squareSize + 1) + squareSize * 0.7);
						}
						else {
							textH[i][j].setPosition(indent + j * (squareSize + 1) + squareSize * 0.81, indent + i * (squareSize + 1) + squareSize * 0.7);
						}
						window->draw(textH[i][j]);
					}
				}
				//Lines for parent indicators.
				if (graph[i * w + j]->parent != nullptr) {
					lines[i * w + j][0].color = sf::Color::Black;
					lines[i * w + j][1].color = sf::Color::Black;
					//Setting one end of the line to the center of the cell.
					lines[i * w + j][0].position.x = indent + j * (squareSize + 1) + squareSize * 0.5;
					lines[i * w + j][0].position.y = indent + i * (squareSize + 1) + squareSize * 0.5;
					//Length of the line.
					float radius = 0.29;
					//Setting the position of the other end of the line, pointing at it's parent cell's location.
					//Top-Left
					if (graph[i * w + j]->i < graph[i * w + j]->parent->i && graph[i * w + j]->j < graph[i * w + j]->parent->j) {
						lines[i * w + j][1].position.x = indent + j * (squareSize + 1) + squareSize * 0.5 + cosf(45 * PI / 180) * 180 / PI * radius;
						lines[i * w + j][1].position.y = indent + i * (squareSize + 1) + squareSize * 0.5 + sinf(45 * PI / 180) * 180 / PI * radius;
					}
					//Top-Right
					else if (graph[i * w + j]->i < graph[i * w + j]->parent->i && graph[i * w + j]->j > graph[i * w + j]->parent->j) {
						lines[i * w + j][1].position.x = indent + j * (squareSize + 1) + squareSize * 0.5 + cosf(135 * PI / 180) * 180 / PI * radius;
						lines[i * w + j][1].position.y = indent + i * (squareSize + 1) + squareSize * 0.5 + sinf(135 * PI / 180) * 180 / PI * radius;
					}
					//Top
					else if (graph[i * w + j]->i < graph[i * w + j]->parent->i) {
						lines[i * w + j][1].position.x = indent + j * (squareSize + 1) + squareSize * 0.5 + cosf(90 * PI / 180.f) * 180 / PI * radius;
						lines[i * w + j][1].position.y = indent + i * (squareSize + 1) + squareSize * 0.5 + sinf(90 * PI / 180.f) * 180 / PI * radius;
					}
					//Bottom-Left
					else if (graph[i * w + j]->i > graph[i * w + j]->parent->i && graph[i * w + j]->j < graph[i * w + j]->parent->j) {
						lines[i * w + j][1].position.x = indent + j * (squareSize + 1) + squareSize * 0.5 + cosf(315 * PI / 180) * 180 / PI * radius;
						lines[i * w + j][1].position.y = indent + i * (squareSize + 1) + squareSize * 0.5 + sinf(315 * PI / 180) * 180 / PI * radius;
					}
					//Bottom-Right
					else if (graph[i * w + j]->i > graph[i * w + j]->parent->i && graph[i * w + j]->j > graph[i * w + j]->parent->j) {
						lines[i * w + j][1].position.x = indent + j * (squareSize + 1) + squareSize * 0.5 + cosf(225 * PI / 180) * 180 / PI * radius;
						lines[i * w + j][1].position.y = indent + i * (squareSize + 1) + squareSize * 0.5 + sinf(225 * PI / 180) * 180 / PI * radius;
					}
					//Bottom
					else if (graph[i * w + j]->i > graph[i * w + j]->parent->i) {
						lines[i * w + j][1].position.x = indent + j * (squareSize + 1) + squareSize * 0.5 + cosf(270 * PI / 180) * 180 / PI * radius;
						lines[i * w + j][1].position.y = indent + i * (squareSize + 1) + squareSize * 0.5 + sinf(270 * PI / 180) * 180 / PI * radius;
					}
					//Left
					else if (graph[i * w + j]->j < graph[i * w + j]->parent->j) {
						lines[i * w + j][1].position.x = indent + j * (squareSize + 1) + squareSize * 0.5 + cosf(0 * PI / 180) * 180 / PI * radius;
						lines[i * w + j][1].position.y = indent + i * (squareSize + 1) + squareSize * 0.5 + sinf(0 * PI / 180) * 180 / PI * radius;
					}
					//Right
					else if (graph[i * w + j]->j > graph[i * w + j]->parent->j) {
						lines[i * w + j][1].position.x = indent + j * (squareSize + 1) + squareSize * 0.5 + cosf(180 * PI / 180) * 180 / PI * radius;
						lines[i * w + j][1].position.y = indent + i * (squareSize + 1) + squareSize * 0.5 + sinf(180 * PI / 180) * 180 / PI * radius;
					}
					window->draw(lines[i * w + j], 2, sf::Lines);
				}
				//Circle shapes for open/closed set indicators.
				//If the node has a parent or it is the starting node:
				if (graph[i * w + j]->parent != nullptr || graph[i * w + j] == startNode) {
					//If it is in the open set:
					if (graph[i * w + j]->open) {
						//Setting background color for the circle depending on the color of the cell it is on.
						if (graph[i * w + j] == startNode) {
							circles[i][j].setFillColor(sf::Color(204, 255, 204));
						}
						else if (graph[i * w + j] == endNode) {
							circles[i][j].setFillColor(sf::Color(255, 204, 204));
						}
						else {
							circles[i][j].setFillColor(sf::Color(200, 200, 200));
						}
						window->draw(circles[i][j]);
					}
					//If it is in the closed set:
					else if (graph[i * w + j]->closed || graph[i * w + j] == startNode) {
						//Fill it black.
						circles[i][j].setFillColor(sf::Color::Black);
						window->draw(circles[i][j]);
					}
				}
			}
		}
	}
	//Neighbor node indicator.
	if (neighborNode != nullptr) {
		neighborNodeIndicator.setPosition(indent + neighborNode->j * (squareSize + 1), indent + neighborNode->i * (squareSize + 1));
		window->draw(neighborNodeIndicator);
	}
	if (currentNode != nullptr) {
		currentNodeIndicator.setPosition(indent + currentNode->j * (squareSize + 1), indent + currentNode->i * (squareSize + 1));
		window->draw(currentNodeIndicator);
	}
	window->display();
}

//These render functions are exact copies of their non rendering counterparts except I call 
//sf::sleep() and render() periodically when interesting stuff is happening in the algorithms.

void Pathfinding::aStarVectorRendering()
{
	startNode->g = 0;
	currentNode = startNode;
	currentNode->open = true;
	openVector.push_back(currentNode);
	render();
	for (auto node : graph) {
		if (node != nullptr) {
			if (node != startNode && node != endNode) {
				calculateH(node);
			}
		}
	}
	while (true) {
		currentNode->open = false;
		removeFromVector(openVector, currentNode);
		currentNode->closed = true;
		closedVector.push_back(currentNode);
		for (auto node : currentNode->neighbors) {		
			if (currentNode->i == 4 && currentNode->j == 5) {
				int asdf = 0;
			}
			neighborNode = node;
			sf::sleep(sf::milliseconds(delay));
			render();
			if (node->closed) {		
				continue;
			}
			else if (node->open) {
				if (currentNode->g + calculateMovementValue(currentNode, node) < node->g) {		
					node->parent = currentNode;													
					calculateG(node, calculateMovementValue(currentNode, node));
					calculateF(node);
					sf::sleep(sf::milliseconds(delay));
					render();
				}
				continue;
			}
			node->parent = currentNode;
			calculateG(node, calculateMovementValue(currentNode, node));
			calculateF(node);
			node->open = true;
			openVector.push_back(node);
			sf::sleep(sf::milliseconds(delay));
			render();
			if (node == endNode) {
				node->open = false;
				node->closed = true;
				sf::sleep(sf::milliseconds(delay));
				render();
				return;
			}
		}
		neighborNode = nullptr;
		currentNode = lowestFNodeInVector(openVector);
		sf::sleep(sf::milliseconds(delay));
		render();
	}
}

void Pathfinding::aStarPriorityQueueRendering()
{
	startNode->g = 0;
	currentNode = startNode;
	currentNode->open = true;
	openPriorityQueue.push(currentNode);
	render();
	for (auto node : graph) {
		if (node != nullptr) {
			if (node != startNode && node != endNode) {
				calculateH(node);
			}
		}
	}
	while (true) {
		currentNode->open = false;
		openPriorityQueue.pop();
		currentNode->closed = true;
		closedPriorityQueue.push(currentNode);
		for (auto node : currentNode->neighbors) {		
			neighborNode = node;
			if (node->closed) {		
				continue;
			}
			else if (node->open) {
				if (currentNode->g + calculateMovementValue(currentNode, node) < node->g) {		
					node->parent = currentNode;													
					calculateG(node, calculateMovementValue(currentNode, node));
					calculateF(node);
					sf::sleep(sf::milliseconds(delay));
					render();
				}
				continue;
			}
			node->parent = currentNode;
			sf::sleep(sf::milliseconds(delay));
			render();
			calculateG(node, calculateMovementValue(currentNode, node));
			calculateF(node);
			node->open = true;
			openPriorityQueue.push(node);
			sf::sleep(sf::milliseconds(delay));
			render();
			if (node == endNode) {
				node->open = false;
				node->closed = true;
				sf::sleep(sf::milliseconds(delay));
				render();
				return;
			}
		}
		neighborNode = nullptr;
		currentNode = openPriorityQueue.top();
		sf::sleep(sf::milliseconds(delay));
		render();
	}
}

void Pathfinding::dijkstraVectorRendering()
{
	startNode->g = 0;
	currentNode = startNode;
	currentNode->open = true;
	openVector.push_back(currentNode);
	int movementValue;
	while (true) {
		if (currentNode == nullptr) {	
			break;
		}
		currentNode->open = false;
		removeFromVector(openVector, currentNode);
		currentNode->closed = true;
		closedVector.push_back(currentNode);
		for (auto node : currentNode->neighbors) {
			neighborNode = node;
			sf::sleep(sf::milliseconds(delay));
			render();
			if (node->closed) {
				continue;
			}
			else if (node->open) {
				movementValue = calculateMovementValue(currentNode, node);
				if (currentNode->g + movementValue < node->g) {
					node->parent = currentNode;
					calculateG(node, calculateMovementValue(currentNode, node));
					sf::sleep(sf::milliseconds(delay));
					render();
				}
				continue;
			}
			node->parent = currentNode;
			calculateG(node, calculateMovementValue(currentNode, node));
			node->open = true;
			openVector.push_back(node);
			sf::sleep(sf::milliseconds(delay));
			render();
		}
		neighborNode = nullptr;
		currentNode = lowestFNodeInVector(openVector);
		sf::sleep(sf::milliseconds(delay));
		render();
	}
}

void Pathfinding::dijkstraPriorityQueueRendering()
{
	startNode->g = 0;
	currentNode = startNode;
	currentNode->open = true;
	openPriorityQueue.push(currentNode);
	int movementValue;
	while (true) {
		if (currentNode == nullptr) {	
			break;
		}
		currentNode->open = false;
		openPriorityQueue.pop();
		currentNode->closed = true;
		closedPriorityQueue.push(currentNode);
		for (auto node : currentNode->neighbors) {
			neighborNode = node;
			sf::sleep(sf::milliseconds(delay));
			render();
			if (node->closed) {
				continue;
			}
			else if (node->open) {
				movementValue = calculateMovementValue(currentNode, node);
				if (currentNode->g + movementValue < node->g) {
					node->parent = currentNode;
					calculateG(node, calculateMovementValue(currentNode, node));
					sf::sleep(sf::milliseconds(delay));
					render();
				}
				continue;
			}
			node->parent = currentNode;
			calculateG(node, calculateMovementValue(currentNode, node));
			node->open = true;
			openPriorityQueue.push(node);
			sf::sleep(sf::milliseconds(delay));
			render();
		}
		neighborNode = nullptr;
		if (openPriorityQueue.empty()) {
			currentNode = nullptr;
		}
		else {
			currentNode = openPriorityQueue.top();
		}
		sf::sleep(sf::milliseconds(delay));
		render();
	}
}

void Pathfinding::backTracingRendering()
{
	currentNode = endNode;
	do {
		currentNode->path = true;
		currentNode = currentNode->parent;
		sf::sleep(sf::milliseconds(delay));
		render();
	} while (currentNode != startNode);
}

void Pathfinding::printMap()
{
	//Printing to console.
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			std::cout << fileMap[i][j];
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}
