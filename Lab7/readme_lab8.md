Lab 8 - Robot Maze
===

Objective
---
To be the fastest robot through the maze without hitting any walls

![THE MAZE](./images/maze_diagram.png)

Strategy
---
Though my strategy changed and evolved as I spent more time hands on with the robot, the overall concept stayed the same. I took Dr. Coulston's advice and operated my robot like a state machine. The first state is the straight path then left turn. The next stage begins when the robot loses the right wall, entering the "horseshoe turn". The last state is much the same as the first: straight then left, however there is no need to watch out for the horseshoe. So I broke up the maze into three states that Doomba had to recognize and successfully navigate.

On a lower level, my strategy for sensing walls was different than the other competitors. Most had a gradient of `#define`d values they would check for and make decisions from. I chose to use a big red button (see wiring picture below) to calibrate the sensors *every time the robot started up*. This eliminated the variations of sensor inputs based on room lighting, temperature, shadows (for the most part), and even different reflective properties of the walls in different mazes.

The basic movement pattern for Doomba was

1. Read sensors
2. Move according to location in maze
3. Where am I?
4. Update state if necessary

Implementation
---

![WIRING](./images/wiring.jpg)
