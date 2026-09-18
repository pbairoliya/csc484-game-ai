#include "indoorEnv.h"
#include <vector>

using namespace std;

enum treeNodeString{
    Seek_Enemy,
    Pathfind_TV,
    Kill_Enemy,
    Arrive_Enemy,
    Back_Dance,
    Wander,
    Sit
};
class Task{
    public:
    treeNodeString nodeString;
    Boid character;
    vector<Task*> children;

    Task() {
        children.reserve(3);
    }
    Task(Boid & character,treeNodeString nodeString ): character(character), nodeString(nodeString) {
        children.reserve(3);
    }

    virtual ~Task(){
        for (auto & child:children){
            delete child;
        }
    }
    virtual bool run(IndoorEnvironment & environment) = 0;

    void addChild(Task*child){
        children.push_back(child);
    }


};