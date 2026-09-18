# CSC 484 — Building Game AI

Coursework from **NC State University, Spring 2024**. C++ with SFML.
Published later, so commit dates are the publication date, not when the work was done.

An agent that moves, plans, and decides — built in three layers over the semester, each one
consuming the layer below.

## The layers

**Steering** (`steeringBehavior.cpp`) — the movement primitives. Kinematic arrive, align, wander,
and flocking, all producing a `Steering` struct of linear and angular acceleration rather than
setting position directly, so behaviours compose by summing rather than fighting.

**Pathfinding** (`graph.cpp`, `pathfinding.h`, `heurisitic.cpp`) — A\* over a graph, with the
heuristic pulled out as its own unit so admissible and inadmissible heuristics can be swapped and
compared. `assets/envGraph.txt` is the baked navigation graph for the room, ~9,000 lines of it.

**Decision making** (`decisionTree.h`, `decisionTreeNode.h`) — a decision tree whose leaves return
actions that the steering layer then executes. `indoorEnv.cpp` models the room the agent lives in —
walls, obstacles, and the objects it reacts to.

`main.cpp` (726 lines) wires all three together into the SFML render loop: the tree decides, the
pathfinder plans a route, and the steering behaviours actually move the boid along it.

## Building

SFML is a dependency and is **not** vendored here (the original folder carried ~55MB of it).
Install it, then:

```bash
brew install sfml            # or your platform's package manager
cd src && g++ -std=c++17 *.cpp -o game \
  -lsfml-graphics -lsfml-window -lsfml-system
./game
```

Assets (`boid.png`, `food.png`, `bed.png`, `Arial.ttf`, `envGraph.txt`) live in `assets/` and the
program expects to find them relative to the working directory.
