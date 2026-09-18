#include "indoorEnv.h"
#include "structs.h"

using namespace std;
class DecisionTreeNode
{
public:
    string condition;
    bool isDecision = false;
    bool isAction = false;
    DecisionTreeNode *trueBranch;
    DecisionTreeNode *falseBranch;

    DecisionTreeNode(string condition) : condition(condition), trueBranch(nullptr), falseBranch(nullptr) {}

    /**
     * Recursively iterates through through the tree
     * @param environment The indoor environment
     * @param boid The boid character
     */
    DecisionTreeNode *makeDecision(IndoorEnvironment &environment, Boid &boid)
    {
        if (trueBranch == nullptr && falseBranch == nullptr)
        {

            return this;
        }

        bool isHungry = boid.hunger >= 100;
        bool needSleep = boid.tiredness >= 100;
        bool cantDance = boid.skillLevel < 10;

        if (condition == "Tired?")
        {
            if (needSleep)
            {
                return trueBranch->makeDecision(environment, boid);
            }
            else
            {
                return falseBranch->makeDecision(environment, boid);
            }
        }
        else if (condition == "Hungry?")
        {
            if (isHungry)
            {
                return trueBranch->makeDecision(environment, boid);
            }
            else
            {
                return falseBranch->makeDecision(environment, boid);
            }
        }
        else if (condition == "Can't Dance?")
        {
            if (cantDance)
            {
                // Go practice
                return trueBranch->makeDecision(environment, boid);
            }
            else
            {
                // Go dance to increase skill level
                return falseBranch->makeDecision(environment, boid);
            }
        }

        return nullptr;
    }

    std::string toString()
    {
        return this->condition;
    }

    void setTrueNode(DecisionTreeNode *node)
    {
        trueBranch = node;
    }

    void setFalseNode(DecisionTreeNode *node)
    {
        falseBranch = node;
    }
};
