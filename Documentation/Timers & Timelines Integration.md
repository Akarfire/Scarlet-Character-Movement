
Scarlet Character Movement directly integrates with Scarlet Timers & Timelines plugin ([https://github.com/Akarfire/Scarlet_TimersAndTimelines](https://github.com/Akarfire/Scarlet_TimersAndTimelines)), that provides means to create and utilize multipurpose timers and timelines.

Integration is handled inside of [Scarlet Character Movement](Classes/Scarlet%20Character%20Movement.md) component. During begin play *Scarlet Character Movement Component* attempts to locate an existing `TimerController` component, if one does not exist, it creates it.

Access to the `TimerController` is handled using the following method:
```cpp
// Timer controller that handles custom timers and timelines
USTT_TimerController* GetTimerController() { return TimerController; }
```
