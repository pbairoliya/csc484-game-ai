#include <iostream>
#include <fstream>
#include <string>
#include "pathfinding.h"
#include "steeringBehavior.h"
#include "indoorEnv.h"
#include <chrono>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include "decisionTree.h"
#include <vector>

using namespace std;
using namespace std::chrono;

vector<Vertex> path;
Kinematic targetRandom = Kinematic(sf::Vector2f(500, 500), 0.0, sf::Vector2f(0, 0), 0);

Steering decisionSteering(Boid *boid, DecisionResult result, IndoorEnvironment &environment, Graph g)
{
    PathFinding pathFinder;
    PathFollowing pathFollowing;
    boid->boidSteeringState = result.steeringType;
    Vertex endVertexRandom = environment.findClosestVertex(g, targetRandom.position.x, targetRandom.position.y);
    Steering steer = Steering(sf::Vector2f(0.0f, 0.0f), 0.0);
    switch (boid->boidSteeringState)
    {

    // find a random point on the graph to pathfind to over and over again
    case SteeringState::WANDERING:
    {
        bool atTarget = distanceFunc(boid->kinematic.position, targetRandom.position) < 50;
        if (atTarget)
        {
            boid->skillLevel -= 10;

            targetRandom = Kinematic(sf::Vector2f(rand() % 500 + 200, rand() % 500 + 100), 0.0, sf::Vector2f(0, 0), 0);
            endVertexRandom = environment.findClosestVertex(g, targetRandom.position.x, targetRandom.position.y);
        }
        if (boid->skillLevel < 10)
        {
            boid->boidSteeringState = SteeringState::PATHFINDING;
        }

        steer = Steering(sf::Vector2f(0.0f, 0.0f), 0.2);

        Vertex startVertexBoid = environment.findClosestVertex(g, boid->getKinematic().position.x, boid->getKinematic().position.y);

        manhattanHeuristic mh;
        path = pathFinder.aStar(&g, &startVertexBoid, &endVertexRandom, &mh);
        pathFollowing = PathFollowing(path, 1.0f, g);
        steer = pathFollowing.calculate(boid->kinematic, targetRandom);

        break;
    }
    // boid dont do anything for 4 seconds
    case SteeringState::SLEEP:
    {

        boid->timeSitting += 1;

        steer = Steering(sf::Vector2f(0.0f, 0.0f), 0.0);
        if (boid->timeSitting >= 4)
        {
            boid->timeSitting = 0;
            boid->boidSteeringState = SteeringState::WANDERING;
        }
        break;
    }
    case SteeringState::DANCING:
    {
        boid->timeDancing += 1;

        steer = Steering(sf::Vector2f(0.0f, 0.0f), 0.2);
        if (boid->timeDancing >= 30)
        {
            boid->skillLevel += 2;
            boid->timeDancing = 0;
            boid->boidSteeringState = SteeringState::WANDERING;
        }
        break;
    }
    case SteeringState::PATHFINDING:
    {
        sf::Vector2f pantryPos = environment.getBurgerPosition();
        sf::Vector2f bedPos = environment.getBedPosition();
        sf::Vector2f dsPos = environment.getDSPosition();

        bool atTarget = distanceFunc(boid->kinematic.position, result.target.position) < 25;
        if (atTarget)
        {
            bool atPantry = distanceFunc(boid->kinematic.position, pantryPos) < 25;
            bool atBed = distanceFunc(boid->kinematic.position, bedPos) < 25;
            bool atDs = distanceFunc(boid->kinematic.position, dsPos) < 25;
            if (atPantry)
            {
                boid->hunger = 0;
                boid->boidSteeringState = SteeringState::WANDERING;
            }
            else if (atBed)
            {
                boid->tiredness = 0;
                boid->hunger = 0;
                boid->boidSteeringState = SteeringState::SLEEP;
            }
            else if (atDs)
            {
                boid->boidSteeringState = SteeringState::DANCING;
            }
            break;
        }
        // pathfind to target
        else
        {
            Vertex startVertexBoid = environment.findClosestVertex(g, boid->getKinematic().position.x, boid->getKinematic().position.y);
            Vertex endVertexMouse = environment.findClosestVertex(g, result.target.position.x, result.target.position.y);
            manhattanHeuristic mh;
            path = pathFinder.aStar(&g, &startVertexBoid, &endVertexMouse, &mh);
            pathFollowing = PathFollowing(path, 1.0f, g);
            steer = pathFollowing.calculate(boid->kinematic, result.target);
        }
        break;
    }
    }
    return steer;
}
// int main of the program the menu system of the program
int part1()
{
    // Pathfollowing Representation
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE), "Indoor Environment");
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf"))
    {
        std::cerr << "Failed to load font file!" << std::endl;
        return 1;
    }
    sf::Texture boidTexture;
    if (!boidTexture.loadFromFile("boid-sm.png"))
    {
        cerr << "Failed to load boid texture!" << endl;
        return 1;
    }

    vector<crumb> breadcrumbs = vector<crumb>();
    for (int i = 0; i < numberOfCrumbs; i++)
    {
        crumb c(i, 3.0f);
        breadcrumbs.push_back(c);
    }
    // Load textures for bed, TV, and burger
    sf::Texture bedTexture;
    if (!bedTexture.loadFromFile("bed.png"))
    {
        cerr << "Failed to load bed texture!" << endl;
        return 1;
    }

    sf::Texture tvTexture;
    if (!tvTexture.loadFromFile("tv.png"))
    {
        cerr << "Failed to load TV texture!" << endl;
        return 1;
    }

    sf::Texture burgerTexture;
    if (!burgerTexture.loadFromFile("food.png"))
    {
        cerr << "Failed to load burger texture!" << endl;
        return 1;
    }

    sf::Texture pizzaTexture;
    if (!pizzaTexture.loadFromFile("pizza.png"))
    {
        cerr << "Failed to load pizza texture!" << endl;
        return 1;
    }

    // Scale the textures to 30x30 pixels
    bedTexture.setSmooth(true);
    bedTexture.setRepeated(false);
    sf::Sprite bedSprite(bedTexture);
    bedSprite.setScale(30.0f / bedTexture.getSize().x, 30.0f / bedTexture.getSize().y);

    tvTexture.setSmooth(true);
    tvTexture.setRepeated(false);
    sf::Sprite tvSprite(tvTexture);
    tvSprite.setScale(30.0f / tvTexture.getSize().x, 30.0f / tvTexture.getSize().y);

    burgerTexture.setSmooth(true);
    burgerTexture.setRepeated(false);
    sf::Sprite burgerSprite(burgerTexture);
    burgerSprite.setScale(30.0f / burgerTexture.getSize().x, 30.0f / burgerTexture.getSize().y);

    pizzaTexture.setSmooth(true);
    pizzaTexture.setRepeated(false);
    sf::Sprite pizzaSprite(pizzaTexture);
    pizzaSprite.setScale(30.0f / pizzaTexture.getSize().x, 30.0f / pizzaTexture.getSize().y);

    // Create an instance of Boid class and set its position to the top middle of the window
    Boid boid(&window, boidTexture, &breadcrumbs);

    boid.setKinematic(Kinematic(sf::Vector2f(800 / 2, 0), 0, sf::Vector2f(0, 0), 0));
    IndoorEnvironment environment;

    // Load environment from file
    environment.loadFromFile("environment.txt");
    Graph g = environment.generateGraph();

    g.printGraphToFile("envGraph.txt");

    sf::Vector2f redDotPosition; // Position of the red dot
    DecisionTree boidDt(boid, environment);
    DecisionTreeNode *root = new DecisionTreeNode("Tired?");
    DecisionTreeNode *tiredYes = new DecisionTreeNode("Go to bed");
    DecisionTreeNode *hungry = new DecisionTreeNode("Hungry?");
    DecisionTreeNode *hungryYes = new DecisionTreeNode("Get food");
    DecisionTreeNode *lowSkill = new DecisionTreeNode("Can't Dance?");
    DecisionTreeNode *lowSkillYes = new DecisionTreeNode("Go practice");
    DecisionTreeNode *lowSkillNo = new DecisionTreeNode("Wander");

    root->setTrueNode(tiredYes);
    root->setFalseNode(hungry);
    hungry->setTrueNode(hungryYes);
    hungry->setFalseNode(lowSkill);
    lowSkill->setTrueNode(lowSkillYes);
    lowSkill->setFalseNode(lowSkillNo);

    boidDt.root = root;

    window.clear(sf::Color::White);
    for (int i = 0; i < GRID_HEIGHT; ++i)
    {
        for (int j = 0; j < GRID_WIDTH; ++j)
        {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
            if (environment.isObstacle(i, j))
            {
                cell.setFillColor(sf::Color::Black);
            }
            else
            {
                cell.setFillColor(sf::Color::White);
            }
            window.draw(cell);
        }
    }
    boid.move();

    boid.draw();
    window.display();
    bool boidReachedTarget = false;

    steady_clock::time_point lastPrintTime = steady_clock::now(); // Keep track of the last time the tiredness and hunger levels were printed

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        DecisionResult result = boidDt.makeDecision(boid);
        Steering steer = decisionSteering(&boid, result, environment, g);
        boid.kinematic.update(steer.linearAcceleration, steer.angularAcceleration, .32f);

        boid.move();

        // Clear the window
        window.clear(sf::Color::White);

        // Draw the environment grid
        for (int i = 0; i < GRID_HEIGHT; ++i)
        {
            for (int j = 0; j < GRID_WIDTH; ++j)
            {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
                if (environment.isObstacle(i, j))
                {
                    cell.setFillColor(sf::Color::Black);
                }
                else
                {
                    cell.setFillColor(sf::Color::White);
                }
                window.draw(cell);
            }
        }
        // Draw colored rectangles for each room
        sf::RectangleShape roomRect;
        roomRect.setSize(sf::Vector2f(250, 140));
        roomRect.setPosition(0, 0);
        roomRect.setFillColor(sf::Color(255, 0, 0, 128));
        window.draw(roomRect);

        roomRect.setSize(sf::Vector2f(220, 150));
        roomRect.setPosition(580, 0);
        roomRect.setFillColor(sf::Color(0, 255, 0, 128));
        window.draw(roomRect);

        roomRect.setSize(sf::Vector2f(300, 600));
        roomRect.setPosition(0, 310);
        roomRect.setFillColor(sf::Color(0, 0, 255, 128));
        window.draw(roomRect);

        roomRect.setSize(sf::Vector2f(240, 180));
        roomRect.setPosition(560, 420);
        roomRect.setFillColor(sf::Color(255, 255, 0, 128));
        window.draw(roomRect);
        sf::Text danceRoomLabel("Dance Room", font, 18);
        danceRoomLabel.setFillColor(sf::Color::Black);
        danceRoomLabel.setPosition(20, 20);
        window.draw(danceRoomLabel);

        sf::Text kitchenLabel("Kitchen", font, 18);
        kitchenLabel.setFillColor(sf::Color::Black);
        kitchenLabel.setPosition(650, 20);
        window.draw(kitchenLabel);

        sf::Text sleepingRoomLabel("Sleeping Room", font, 18);
        sleepingRoomLabel.setFillColor(sf::Color::Black);
        sleepingRoomLabel.setPosition(20, 350);
        window.draw(sleepingRoomLabel);

        sf::Text tvRoomLabel("TV Room", font, 18);
        tvRoomLabel.setFillColor(sf::Color::Black);
        tvRoomLabel.setPosition(600, 550);

        bedSprite.setPosition(environment.getBedPosition()); // Adjust the position for the bed in the sleeping room
        tvSprite.setPosition(environment.getTVPosition());   // Adjust the position for the TV in the TV room
        burgerSprite.setPosition(environment.getBurgerPosition());
        pizzaSprite.setPosition(sf::Vector2f(environment.getBedPosition().x, environment.getBedPosition().y + 75));

        window.draw(bedSprite);

        // Draw the TV in the TV room

        window.draw(tvSprite);
        window.draw(pizzaSprite);

        // Draw the burger in the kitchen
        window.draw(burgerSprite);
        window.draw(tvRoomLabel);

        if (!path.empty()) // Check if the path is not empty and the boid has not reached the red dot
        {
            // Draw the vertices of the path as yellow circles
            for (const Vertex &vertex : path)
            {
                sf::CircleShape vertexShape(CELL_SIZE / 4.0); // Circle size relative to cell size
                vertexShape.setFillColor(sf::Color::Yellow);
                vertexShape.setPosition(vertex.x - CELL_SIZE / 8.0, vertex.y - CELL_SIZE / 8.0); // Center the circle in the cell
                window.draw(vertexShape);
            }
        }
        // Draw the breadcrumbs
        for (int i = 0; i < numberOfCrumbs; i++)
        {
            breadcrumbs[i].draw(&window);
        }

        // Draw the boid
        boid.draw();

        // Update and print the tiredness and hunger levels every 2 seconds
        steady_clock::time_point now = steady_clock::now();
        duration<double> timeSinceLastPrint = duration_cast<duration<double>>(now - lastPrintTime);
        if (timeSinceLastPrint.count() >= 1.5)
        {
            // Update the tiredness and hunger levels
            boid.updateAttributes();

            lastPrintTime = now;
        }

        // Display everything on the window
        sf::Text statusText;
        statusText.setFont(font);
        statusText.setCharacterSize(16);
        statusText.setFillColor(sf::Color::Red);

        // Construct the status string
        ostringstream statusStream;
        statusStream << fixed << std::setprecision(1);
        statusStream << "Loc: (" << boid.kinematic.position.x << ", " << boid.kinematic.position.y << ") ";
        statusStream << "Tiredness: " << boid.getTiredness() << " ";
        statusStream << "Hunger: " << boid.getHunger() << " ";
        statusStream << "Skill: " << boid.getSkillLevel() << " ";
        // Add other attributes if needed

        // Set the string to the text object
        statusText.setString(statusStream.str());

        // Position the text at the bottom left corner of the window
        statusText.setPosition(10, window.getSize().y - 30);

        // Draw the text on the window
        window.draw(statusText);
        window.display();
    }
    delete root;
    delete tiredYes;
    delete hungry;
    delete hungryYes;
    delete lowSkill;
    delete lowSkillYes;
    delete lowSkillNo;
    return 0;
}

int part2()
{
    // Pathfollowing Representation
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE), "Indoor Environment");
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf"))
    {
        std::cerr << "Failed to load font file!" << std::endl;
        return 1;
    }
    sf::Texture boidTexture;
    if (!boidTexture.loadFromFile("boid-sm.png"))
    {
        cerr << "Failed to load boid texture!" << endl;
        return 1;
    }

    vector<crumb> breadcrumbs = vector<crumb>();
    for (int i = 0; i < numberOfCrumbs; i++)
    {
        crumb c(i, 3.0f);
        breadcrumbs.push_back(c);
    }
    // Load textures for bed, TV, and burger
    sf::Texture bedTexture;
    if (!bedTexture.loadFromFile("bed.png"))
    {
        cerr << "Failed to load bed texture!" << endl;
        return 1;
    }

    sf::Texture tvTexture;
    if (!tvTexture.loadFromFile("tv.png"))
    {
        cerr << "Failed to load TV texture!" << endl;
        return 1;
    }

    sf::Texture burgerTexture;
    if (!burgerTexture.loadFromFile("food.png"))
    {
        cerr << "Failed to load burger texture!" << endl;
        return 1;
    }

    sf::Texture pizzaTexture;
    if (!pizzaTexture.loadFromFile("pizza.png"))
    {
        cerr << "Failed to load pizza texture!" << endl;
        return 1;
    }

    // Scale the textures to 30x30 pixels
    bedTexture.setSmooth(true);
    bedTexture.setRepeated(false);
    sf::Sprite bedSprite(bedTexture);
    bedSprite.setScale(30.0f / bedTexture.getSize().x, 30.0f / bedTexture.getSize().y);

    tvTexture.setSmooth(true);
    tvTexture.setRepeated(false);
    sf::Sprite tvSprite(tvTexture);
    tvSprite.setScale(30.0f / tvTexture.getSize().x, 30.0f / tvTexture.getSize().y);

    burgerTexture.setSmooth(true);
    burgerTexture.setRepeated(false);
    sf::Sprite burgerSprite(burgerTexture);
    burgerSprite.setScale(30.0f / burgerTexture.getSize().x, 30.0f / burgerTexture.getSize().y);

    pizzaTexture.setSmooth(true);
    pizzaTexture.setRepeated(false);
    sf::Sprite pizzaSprite(pizzaTexture);
    pizzaSprite.setScale(30.0f / pizzaTexture.getSize().x, 30.0f / pizzaTexture.getSize().y);

    // Create an instance of Boid class and set its position to the top middle of the window
    Boid boid(&window, boidTexture, &breadcrumbs);

    boid.setKinematic(Kinematic(sf::Vector2f(800 / 2, 0), 0, sf::Vector2f(0, 0), 0));
    IndoorEnvironment environment;

    // Load environment from file
    environment.loadFromFile("environment.txt");
    Graph g = environment.generateGraph();

    g.printGraphToFile("envGraph.txt");

    sf::Vector2f redDotPosition; // Position of the red dot
    DecisionTree boidDt(boid, environment);
    DecisionTreeNode *root = new DecisionTreeNode("Tired?");
    DecisionTreeNode *tiredYes = new DecisionTreeNode("Go to bed");
    DecisionTreeNode *hungry = new DecisionTreeNode("Hungry?");
    DecisionTreeNode *hungryYes = new DecisionTreeNode("Get food");
    DecisionTreeNode *lowSkill = new DecisionTreeNode("Can't Dance?");
    DecisionTreeNode *lowSkillYes = new DecisionTreeNode("Go practice");
    DecisionTreeNode *lowSkillNo = new DecisionTreeNode("Wander");

    root->setTrueNode(tiredYes);
    root->setFalseNode(hungry);
    hungry->setTrueNode(hungryYes);
    hungry->setFalseNode(lowSkill);
    lowSkill->setTrueNode(lowSkillYes);
    lowSkill->setFalseNode(lowSkillNo);

    boidDt.root = root;
    window.clear(sf::Color::White);
    for (int i = 0; i < GRID_HEIGHT; ++i)
    {
        for (int j = 0; j < GRID_WIDTH; ++j)
        {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
            if (environment.isObstacle(i, j))
            {
                cell.setFillColor(sf::Color::Black);
            }
            else
            {
                cell.setFillColor(sf::Color::White);
            }
            window.draw(cell);
        }
    }
    boid.move();

    boid.draw();
    window.display();
    bool boidReachedTarget = false;

    steady_clock::time_point lastPrintTime = steady_clock::now(); // Keep track of the last time the tiredness and hunger levels were printed

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        DecisionResult result = boidDt.makeDecision(boid);
        Steering steer = decisionSteering(&boid, result, environment, g);
        boid.kinematic.update(steer.linearAcceleration, steer.angularAcceleration, .32f);
        environment.setCharacterPos(boid.kinematic.position);
        boid.move();
        //cout<<environment.getCharacterPos().x<<"       "<<environment.getCharacterPos().y<<endl;
        // Clear the window
        window.clear(sf::Color::White);

        // Draw the environment grid
        for (int i = 0; i < GRID_HEIGHT; ++i)
        {
            for (int j = 0; j < GRID_WIDTH; ++j)
            {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
                if (environment.isObstacle(i, j))
                {
                    cell.setFillColor(sf::Color::Black);
                }
                else
                {
                    cell.setFillColor(sf::Color::White);
                }
                window.draw(cell);
            }
        }
        // Draw colored rectangles for each room
        sf::RectangleShape roomRect;
        roomRect.setSize(sf::Vector2f(250, 140));
        roomRect.setPosition(0, 0);
        roomRect.setFillColor(sf::Color(255, 0, 0, 128));
        window.draw(roomRect);

        roomRect.setSize(sf::Vector2f(220, 150));
        roomRect.setPosition(580, 0);
        roomRect.setFillColor(sf::Color(0, 255, 0, 128));
        window.draw(roomRect);

        roomRect.setSize(sf::Vector2f(300, 600));
        roomRect.setPosition(0, 310);
        roomRect.setFillColor(sf::Color(0, 0, 255, 128));
        window.draw(roomRect);

        roomRect.setSize(sf::Vector2f(240, 180));
        roomRect.setPosition(560, 420);
        roomRect.setFillColor(sf::Color(255, 255, 0, 128));
        window.draw(roomRect);
        sf::Text danceRoomLabel("Dance Room", font, 18);
        danceRoomLabel.setFillColor(sf::Color::Black);
        danceRoomLabel.setPosition(20, 20);
        window.draw(danceRoomLabel);

        sf::Text kitchenLabel("Kitchen", font, 18);
        kitchenLabel.setFillColor(sf::Color::Black);
        kitchenLabel.setPosition(650, 20);
        window.draw(kitchenLabel);

        sf::Text sleepingRoomLabel("Sleeping Room", font, 18);
        sleepingRoomLabel.setFillColor(sf::Color::Black);
        sleepingRoomLabel.setPosition(20, 350);
        window.draw(sleepingRoomLabel);

        sf::Text tvRoomLabel("TV Room", font, 18);
        tvRoomLabel.setFillColor(sf::Color::Black);
        tvRoomLabel.setPosition(600, 550);

        bedSprite.setPosition(environment.getBedPosition()); // Adjust the position for the bed in the sleeping room
        tvSprite.setPosition(environment.getTVPosition());   // Adjust the position for the TV in the TV room
        burgerSprite.setPosition(environment.getBurgerPosition());
        pizzaSprite.setPosition(sf::Vector2f(environment.getBedPosition().x, environment.getBedPosition().y + 75));

        window.draw(bedSprite);

        // Draw the TV in the TV room

        window.draw(tvSprite);
        window.draw(pizzaSprite);

        // Draw the burger in the kitchen
        window.draw(burgerSprite);
        window.draw(tvRoomLabel);

        if (!path.empty()) // Check if the path is not empty and the boid has not reached the red dot
        {
            // Draw the vertices of the path as yellow circles
            for (const Vertex &vertex : path)
            {
                sf::CircleShape vertexShape(CELL_SIZE / 4.0); // Circle size relative to cell size
                vertexShape.setFillColor(sf::Color::Yellow);
                vertexShape.setPosition(vertex.x - CELL_SIZE / 8.0, vertex.y - CELL_SIZE / 8.0); // Center the circle in the cell
                window.draw(vertexShape);
            }
        }
        // Draw the breadcrumbs
        for (int i = 0; i < numberOfCrumbs; i++)
        {
            breadcrumbs[i].draw(&window);
        }
        // Draw the boid
        boid.draw();

        // Update and print the tiredness and hunger levels every 2 seconds
        steady_clock::time_point now = steady_clock::now();
        duration<double> timeSinceLastPrint = duration_cast<duration<double>>(now - lastPrintTime);
        if (timeSinceLastPrint.count() >= 1.5)
        {
            // Update the tiredness and hunger levels
            boid.updateAttributes();

            lastPrintTime = now;
        }

        // Display everything on the window
        sf::Text statusText;
        statusText.setFont(font);
        statusText.setCharacterSize(16);
        statusText.setFillColor(sf::Color::Red);

        // Construct the status string
        ostringstream statusStream;
        statusStream << fixed << std::setprecision(1);
        statusStream << "Loc: (" << boid.kinematic.position.x << ", " << boid.kinematic.position.y << ") ";
        statusStream << "Tiredness: " << boid.getTiredness() << " ";
        statusStream << "Hunger: " << boid.getHunger() << " ";
        statusStream << "Skill: " << boid.getSkillLevel() << " ";
        // Add other attributes if needed

        // Set the string to the text object
        statusText.setString(statusStream.str());

        // Position the text at the bottom left corner of the window
        statusText.setPosition(10, window.getSize().y - 30);

        // Draw the text on the window
        window.draw(statusText);
        window.display();
    }
    delete root;
    delete tiredYes;
    delete hungry;
    delete hungryYes;
    delete lowSkill;
    delete lowSkillYes;
    delete lowSkillNo;
    return 0;
}

int main()
{
    // Declaration of variable to store user's choice
    int choice = 0;

    // Loop to repeatedly prompt the user for choices until they choose to exit
    while (choice != 3)
    {
        // Prompt the user to choose between different parts or to exit
        cout << "Choose between part1 - 1, part2 -2"<<endl;
        cin >> choice; // Read user's choice from input

        // Check the user's choice and execute the corresponding part
        if (choice == 1)
        {
            part1();
        }
        else if (choice == 2)
        {
            part2();
        }

    }
    return 0;
}