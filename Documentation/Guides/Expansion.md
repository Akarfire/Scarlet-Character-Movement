
**WIP**
*TO DO: Add more details to the expansion guide*

### Recommended Code Structure

#### Header File

```cpp
/*
 * Enum for the States of the State Machine
 * First value must always be None, as its considered as non-existance by the state machine
*/
UENUM(BlueprintType)
enum class ESCM_MyMovementStates : uint8
{
	None UMETA(DisplayName = "None"),
	MovementState_1 UMETA(DisplayName = "MovementState_1"),
	MovementState_2 UMETA(DisplayName = "MovementState_2"),
	LowerLevel UMETA(DisplayName = "LowerLevel")
};
```

```cpp
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_MyMovementState_1 : public USCM_MovementStateBase
{
	GENERATED_BODY()
	
	// Parameters
	float Param = 400.0;

public:

	// Constructor
	USCM_MyMovementState_1() {}

	// Called when any parameter value is changed
	UFUNCTION()
	void OnParameterValueChanged(const FName& ParameterName);
	
	UFUNCTION()
	void OnTimerIsOver(FName TimerName);

	// Registering parameters
	virtual void SetupParameters_Implementation() override;

	// Called when the state is made active
	virtual void EnterState_Implementation() override;

	// Called once the active state is changed to a different one
	virtual void ExitState_Implementation() override;

	// Called every state machine update when the state is active
	virtual void UpdateState_Implementation(float DeltaTime) override;
};
```

```cpp
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_MyMovementLayer : public USCM_MovementStateMachine
{
	GENERATED_BODY()

protected:
	// Control parameters
	float ControlParameter = 100.f;

public:

	// Constructor
	USCM_MyMovementLayer() 
	{ 
		StateDisplayNames = 
		{ "None", "MovementState_1", "MovementState_2", "LowerLevel" }; 
	}

	// Called when the state machine is initialized (after the main init)
	virtual void OnInitStateMachine_Implementation() override
	{
		// Registering States
		AddNewState( (uint8)ESCM_MyMovementStates::MovementState_1, 
					 USCM_MyMovementState_1::StaticClass());
		AddNewState( (uint8)ESCM_MyMovementStates::MovementState_2,
					 USCM_MyMovementState_2::StaticClass());

		//Alternatively you can use automatic Transition Registration:
		AutoTransitionRegistration(StateDisplayNames, "Condition_", "_");

		// Set Default State
		ForceCallStateTransition( (uint8)ESCM_MyMovementStates::LowerLevel);
	}

	// Called every time the state machine is updated (after the main update)
	virtual void OnUpdateStateMachine_Implementation(float DeltaTime) override
	{
		Super::OnUpdateStateMachine_Implementation(DeltaTime);
	}

	// Called every time the state is changed
	virtual void OnStateChangedInternal_Implementation(uint8 OldState, uint8 NewState) {}

	// Returns a pointer to the state that contains the lower level of the movement state machine stack
	virtual USSM_NestedStateMachineState* GetLowerLayerContainerState_Implementation() 
	{ 
		return Cast<USSM_NestedStateMachineState>(GetState((uint8)USCM_MyMovementState_2::LowerLevel)); 
	}

	// Needed to register state-machine level movement parameters (for state transition conditions)
	virtual void OnSetupParameters_Implementation()
	{
		// Register control parameters here
		SM->RegisterFloatParameter( "ControlParameter", 
									 ControlParameter, 
									 true, 
									 this, 
									 "OnParameterValueChanged");
		// ...

		// Dynamic gates
		GetScarletMovement()->RegisterDynamicGate(
			"CanDoThing", 
			 ESCM_DynamicGateRule::And, 
			 true);
			 
		GetScarletMovement()->RegisterDynamicGate(
			"StopThin", 
			 ESCM_DynamicGateRule::Or, 
			 false);
		// ...

		// Timers & Timelines
		// ...
	}

	UFUNCTION()
	void OnParameterValueChanged(const FName& ParameterName) {}

	UFUNCTION()
	void OnTimerIsOver(FName InTimerName) {}

	// Fired when this movement layer is entered
	virtual void OnLayerEntered_Implementation()
	{
	}

	//  Fired when this movement layer is exited
	virtual void OnLayerExit_Implementation()
	{
	}

	// TRANSITION CONDITIONS
	// Must be a UFUNCTION, will not work otherwise, and no, it will not crash, you will just be stuck there with no real signs of errors, so yeah

	// Condition for transition from state One to state Two
	UFUNCTION()
	bool Condition_LowerLevel_MovementState_1() 
	{ 
		return	GetScarletMovement()->GetBoolInputValue("Input")
				&& GetScarletMovement()->GetDynamicGateValue("CanDoThing")
	}
	// ...

};
```

#### Source File

```cpp
// Registering parameters
void USCM_MyMovementState_1::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	SM->RegisterFloatParameter("Param", Param, true, this, "OnParameterValueChanged");

	// Timers
	USTT_TimerController* TC = SM->GetTimerController();

	TC->CreateTimer("TimerName", RollDelay, false, false);
	TC->SubscribeToTimer("TimerName", this, "OnTimerIsOver");

	// Dynamic gates
	if (SM->IsDynamicGateValid("CanDoThing"))
		SM->SetDynamicGateNamedValue("CanDoThing", "NotRolling", true);
}

// Called when any parameter value is changed
void USCM_MyMovementState_1::OnParameterValueChanged(const FName& ParameterName)
{
	if (ParameterName == "Param")
		Param = GetScarletMovement()->GetFloatParameterValue("Param");

}

void USCM_MyMovementState_1::OnTimerIsOver(FName TimerName)
{
	if (TimerName == "TimerName")
	{
		//...
	}
}

void USCM_MyMovementState_1::EnterState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	CM->SetMovementMode(EMovementMode::MOVE_Walking);
	//...
	
	// Dynamic gates
	if (GetScarletMovement()->IsDynamicGateValid("CanDoThing"))
		GetScarletMovement()->SetDynamicGateNamedValue("CanDoThing", "NotDoingTheThing", false);

	// Startin timers or something else
	GetScarletMovement()->GetTimerController()->StartTimer("TimerName");
}

void USCM_MyMovementState_1::ExitState_Implementation()
{
	// Reverting Character Movement parameters
	// GetCharacterMovement()->GroundFriction = ...

	// Dynamic gates
	if (GetScarletMovement()->IsDynamicGateValid("CanDoThing"))
		GetScarletMovement()->SetDynamicGateNamedValue("CanDoThing", "NotDoingTheThing", true);
}

void USCM_MyMovementState_1::UpdateState_Implementation(float DeltaTime)
{
	// Executed every tick
}
```