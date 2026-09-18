#include "decisionTreeNode.h" // Include the DecisionTreeNode header file
#include "structs.h"
#include "indoorEnv.h"
#include <iostream>
#include <utility> // Include for std::pair

using namespace std;

// Define a struct to hold both the steering type, character state, and the target
struct DecisionResult
{
    SteeringState steeringType;
    CharacterState characterState;
    Kinematic target;

    // Default constructor
    DecisionResult()
        : steeringType(SteeringState::SLEEP),
          characterState(CharacterState::SLEEPING),
          target(Kinematic(sf::Vector2f(0, 0), 0.0, sf::Vector2f(0, 0), 0.0)) {}
};

class DecisionTree
{
public:
    DecisionTreeNode *root;
    IndoorEnvironment env;
    Boid character;

    DecisionTree(Boid character, IndoorEnvironment env)
        : root(nullptr), character(character), env(env) {}

    // Modify makeDecision to return DecisionResult
    DecisionResult makeDecision(Boid &chr)
    {
        if (root == nullptr)
        {
            cout << "No decision tree available." << endl;
            return DecisionResult();
        }

        DecisionTreeNode *actionNode = root->makeDecision(env, chr);
        if (actionNode == nullptr)
        {
            cout << "No decision could be made." << endl;
            return DecisionResult();
        }
        string action = actionNode->condition;

        DecisionResult result;
        if (action == "Get food")
        {
            result.steeringType = SteeringState::PATHFINDING;
            result.characterState = CharacterState::MOVING; // Example character state
            sf::Vector2f pantry = env.getBurgerPosition();
            result.target = Kinematic(sf::Vector2f(pantry.x, pantry.y), 0.0, sf::Vector2f(0, 0), 0.0);
            return result;
        }
        else if (action == "Go to bed")
        {
            result.steeringType = SteeringState::PATHFINDING;
            result.characterState = CharacterState::MOVING; // Example character state
            sf::Vector2f bed = env.getBedPosition();
            result.target = Kinematic(sf::Vector2f(bed.x, bed.y), 0.0, sf::Vector2f(0, 0), 0.0);

            return result;
        }
        else if (action == "Go practice")
        {
            if (distanceFunc(chr.kinematic.position, sf::Vector2f(100, 100)) < 50)
            {
                result.steeringType = SteeringState::DANCING;
            }
            else
            {
                result.steeringType = SteeringState::PATHFINDING;
            }
            result.characterState = CharacterState::MOVING; // Example character state
            sf::Vector2f ds = env.getDSPosition();
            result.target = Kinematic(sf::Vector2f(100, 100), 0.0, sf::Vector2f(0, 0), 0.0);
            return result;
        }
        else if (action == "Wander")
        {
            result.steeringType = SteeringState::WANDERING;
            result.characterState = CharacterState::MOVING;                              // Example character state
            result.target = Kinematic(sf::Vector2f(0, 0), 0.0, sf::Vector2f(0, 0), 0.0); // No specific target for wandering
        }
        else
        {
            result.steeringType = SteeringState::SLEEP;
            result.characterState = CharacterState::SLEEPING;                            // Default character state
            result.target = Kinematic(sf::Vector2f(0, 0), 0.0, sf::Vector2f(0, 0), 0.0); // No specific target for wandering
        }

        return result;
    }

    // Remaining print methods remain unchanged
};
