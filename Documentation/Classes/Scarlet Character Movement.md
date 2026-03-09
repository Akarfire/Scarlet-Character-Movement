
The core component of the system. Responsibilities:
* Handling layer movement state machine architecture;
* Detecting and providing access to `UCharacterMovementComponent` of the character;
* [Timers & Timelines Integration](../Timers%20&%20Timelines%20Integration.md);
* Handling [Parameters](../Parameters.md);
* Handling [Input](../Input.md);
* Handling [Dynamic Gates](../Dynamic%20Gates.md).

---
### Movement State Machine Stack

#### Stack Definition

Movement state machine stack is defined by `MovementStateMachineStackClasses` field and contains [SCM Movement State Machine](SCM%20Movement%20State%20Machine.md) static classes:
```cpp
// Layers of the movement state machine stack, 0 - lowest layer
TArray<TSubclassOf<class USCM_MovementStateMachine>> MovementStateMachineStackClasses;
```
First element of the `MovementStateMachineStackClasses` is the lowest layer, last element - top layer.

#### Stack Initialization

`InitMovementStateMachineStack` method is used to create and correctly initialize Movement State Machine stack using classes, defined in `MovementStateMachineStackClasses`:
```cpp
// Creates and initializes a movement state machine stack
void InitMovementStateMachineStack();
```

Newly created movement state machines are stored inside
```cpp
// Stack of state machines that is used for controlling movement
TArray<class USCM_MovementStateMachine*> MovementStateMachineStack;
```

Initialization sequence works in the following manner:

* Looping over every element in `MovementStateMachineStackClasses`:
	* Creating a new `SCM_MovementStateMachine` object based on the specified class;
	* Calling `MOVEMENTSTATEMACHINE_SetScarletMovement` to link the newly created state machine to this *Scarlet Movement Component*;
	* Calling `InitStateMachine` to initialize *Movement States* and transitions;
	* Calling `SetupParameters` to initialize [Parameters](../Parameters.md), [Timers & Timelines](../Timers%20&%20Timelines%20Integration.md) and [Dynamic Gates](../Dynamic%20Gates.md) in the state machine and all of it's states;
	* Linking state machine layers using `GetLowerLayerContainerState` and `SetEmbeddedStateMachine`.

`RegenerateMovementStateMachineStack` method replaces `MovementStateMachineStackClasses` and calls `InitMovementStateMachineStack`. This method is needed to replace movement state machine stack during runtime.

```cpp
// Replaces movement state machine stack
void RegenerateMovementStateMachineStack(TArray<TSubclassOf<class USCM_MovementStateMachine>> InMovementStateMachineStackClasses);
```

#### Active State

In order to access currently active movement state the following method can be used:
```cpp
// Returns currently active movement state in the movement stack
USCM_MovementStateBase* GetActiveMovementState();
```

*Active State* value is maintained by *Movement State Machines* via `SetActiveState` method, that MUST NOT BE CALLED from anywhere else.

Once the state is changed a `FOnMovementStateChanged` delegate is fired:

```cpp
// Fires off when Movement State has changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMovementStateChanged, FString, NewMovementStateName, USCM_MovementStateBase*, NewMovementState);
```

```cpp
// Fires off when Movement State has changed
UPROPERTY(BlueprintAssignable, Category = "Delegates")
FOnMovementStateChanged OnMovementStateChanged;
```

---
### Character Movement Component

*Scarlet Movement Component* provides access to the owner-character's *Character Movement Component* to *Movement State Machines* and *Movement States* inside them via the following method:

```cpp
// Returns a pointer to the character movement component that is controlled by this component
class UCharacterMovementComponent* GetCharacterMovementComponent();
```

*Character Movement Component* that is associated with this *Scarlet Character Movement Component* can be specified automatically or manually using the following method:
```cpp
// Manually sets character movement component
void SetCharacterMovementComponent(class UCharacterMovementComponent* InComponent);
```

To enable/disable automatic location of the *Character Movement Component* one can modify the following field:
```cpp
// If set to true, attempts to automatically locate an existing character movement component
UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
bool AutoDetectCharacterMovement = true;
```

---
### Begin Play

Logic executed during begin play:
1. Automatic detection of *Character Movement Component* if it is enabled;
2. Location or creation of the *Timer Controller* (for [Timers & Timelines Integration](../Timers%20&%20Timelines%20Integration.md));
3. *Movement State Machine* stack initialization using `InitMovementStateMachineStack`;

### Tick Update

Logic executed every tick:
1. [Input](../Input.md) interpolation is updated;
2. Calling `UpdateStateMachine` on the last element of the `MovementStateMachineStack`.