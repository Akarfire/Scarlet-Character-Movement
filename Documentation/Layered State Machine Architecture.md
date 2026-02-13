### Multi-Layer State Machine Principal

*Every update* transition checks start at the entry point and proceed down through the layers. Every layer is a state machine with it's own current state.

If at the end of the layer's update, it's current state is *Lower Layer*, the lower layer state machine is checked. If any other state of the layer is active, it is picked as the **Resulting Active State**, the checks stop at this layer.

### Example

![](Images/Zaris-Style%20Movement%20Stack.png)