
### Principals

1. In-build Unreal Engine's character movement component is used as the movement driver;
2. State machines are organized in stacks (Layered Architecture);
3. Complete modularity on movement state level.

### 1. Why are we using Unreal Engine's default Character Movement Component

*Short Answer:* In-build character movement component has a lot of must-have features and is optimized to work with engine's systems.

*Long Answer:* Implementing a custom movement component would require an incredible amount of work and testing. The main problems would be: 
1. Proper collision detection and collision handling;
2. Network replication and prediction.

Developing such system was deemed inefficient, thus the default movement component was made the main movement driver.

### 2. Why are we using Layered State Machine Architecture

*What is "Layered State Machine Architecture"* : [Layered State Machine Architecture](Layered%20State%20Machine%20Architecture.md)

Layered state machine architecture accomplishes two main goals:
1. Modularity: insert additional layers to expand movement's functionality without editing default state machines;
2. Minimizes explicit state transitions, which in it's turn simplifies development and support.

### 3. Modularity on State Level



