#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <queue>
#include <array>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//I have only commented the rendering code in this project. The comments for the rest of the code are in my testing project.//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PI 3.14159265359

struct Node {
	Node(int i_, int j_)
		: i(i_), j(j_), f(-1), g(-1), h(-1), path(false), parent(nullptr), open(false), closed(false) {}
	int i, j;			
	int f, g, h;		
	bool path;			
	Node* parent;
	bool open, closed;	
	std::vector<Node*> neighbors;
};

struct LowerFValue
{
	bool operator()(const Node* a, const Node* b) const
	{
		return a->f > b->f;
	}
};

struct LowerGValue
{
	bool operator()(const Node* a, const Node* b) const
	{
		return a->g > b->g;
	}
};

class Pathfinding {
public:
	Pathfinding(sf::RenderWindow* hwnd);
	~Pathfinding();

private:
	static const int w = 12;
	static const int h = 12;

	enum algorithms { Dijkstra, AStar };
	algorithms algorithm;
	enum dataStructures { vector, priorityQueue };
	dataStructures dataStructure;
	bool rendering;
	bool diagonals;

	std::array<std::string, h> fileMap;
	std::vector<Node*> graph;
	std::vector<Node*> openVector;
	std::vector<Node*> closedVector;
	std::priority_queue<Node*, std::vector<Node*>, LowerFValue> openPriorityQueue;			//AStar
	std::priority_queue<Node*, std::vector<Node*>, LowerFValue> closedPriorityQueue;
	//std::priority_queue<Node*, std::vector<Node*>, LowerGValue> openPriorityQueue;		//Dijkstra
	//std::priority_queue<Node*, std::vector<Node*>, LowerGValue> closedPriorityQueue;

	Node* startNode;
	Node* endNode;
	Node* currentNode;
	Node* neighborNode = nullptr;

	void connect(Node* a, Node* b);
	void calculateF(Node* node);
	void calculateG(Node* node, int movementValue);
	void calculateH(Node* node);
	int calculateMovementValue(Node* a, Node* b);
	void generateMap();
	void backTracing();
	
	void aStarVector();
	void aStarPriorityQueue();
	void dijkstraVector();
	void dijkstraPriorityQueue();

	void removeFromVector(std::vector<Node*>& dataStructure, Node* node);		//Vector
	Node* lowestFNodeInVector(std::vector<Node*>& dataStructure);

	//Rendering

	sf::RenderWindow* window;

	const int squareSize = 50;
	const int indent = 0;
	float delay;

	void aStarVectorRendering();
	void aStarPriorityQueueRendering();
	void dijkstraVectorRendering();
	void dijkstraPriorityQueueRendering();

	void backTracingRendering();
	void renderSetup();
	void render();
	void printMap();
	
	sf::RectangleShape squares[h][w];
	sf::RectangleShape currentNodeIndicator;
	sf::RectangleShape neighborNodeIndicator;
	sf::Text textF[h][w];
	sf::Text textG[h][w];
	sf::Text textH[h][w];
	sf::Font font;
	sf::CircleShape circles[h][w];
	std::vector<sf::Vertex*> lines;
};
