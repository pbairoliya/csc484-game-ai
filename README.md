# CSC 484 — Building Game AI

**NC State, Spring 2024.** C++ and SFML. An agent that moves, plans, and decides — three layers,
each one built on the last.

> Commit dates are the publication date; the work is from Spring 2024.

## The three layers

🕹️ **Steering** — `steeringBehavior.cpp`
Arrive, align, wander, flock. Every behaviour returns a `Steering` struct of linear and angular
*acceleration* rather than setting position, so behaviours **compose by summing** instead of
fighting each other for control of the boid.

🗺️ **Pathfinding** — `graph.cpp`, `pathfinding.h`, `heurisitic.cpp`
A\* over a navigation graph, with the heuristic split into its own unit so admissible and
inadmissible ones can be swapped and raced against each other. `assets/envGraph.txt` is the baked
graph for the room — about 9,000 lines of it.

🌲 **Decision making** — `decisionTree.h`, `decisionTreeNode.h`
A decision tree whose leaves emit actions that the steering layer carries out. `indoorEnv.cpp` is
the world it reasons about: walls, obstacles, and the things worth walking toward.

`main.cpp` (726 lines) is where they meet: the tree decides, A\* plans the route, steering walks it.

## Running

SFML is a dependency, deliberately **not** vendored — the original folder carried ~55MB of it.

```bash
brew install sfml
cd src && g++ -std=c++17 *.cpp -o game -lsfml-graphics -lsfml-window -lsfml-system
./game
```

Assets (`boid.png`, `food.png`, `bed.png`, `Arial.ttf`, `envGraph.txt`) live in `assets/`.

---

*More: [CSC 230](https://github.com/pbairoliya/csc230-c-software-tools) ·
[CSC 246](https://github.com/pbairoliya/csc246-operating-systems) ·
[CSC 474](https://github.com/pbairoliya/csc474-network-security) ·
[where it started](https://github.com/pbairoliya/first-code)*
