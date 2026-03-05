// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCM_MovementStateMachine.h"
#include "SCM_MovementStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "SCM_LowerLevelState.h"
#include "SCM_RollingLayer.generated.h"


UENUM(BlueprintType)
enum class ESCM_RollingLayerStates : uint8
{
	None UMETA(DisplayName = "None"),
	Rolling UMETA(DisplayName = "Rolling"),
	LowerLevel UMETA(DisplayName = "LowerLevel")
};

// STATES

/**
 * Rolling state
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_Rolling : public USCM_MovementStateBase
{
	GENERATED_BODY()

protected:
	float CachedGroundFriction = 0.f;
	float CachedGravityScale = 1.f;

protected:

	// Parameters
	float HorizontalVelocityConservation = 0.25f;
	float RollBoost = 1500.f;
	float RollDelay = 0.1f;
	float RollDuration = 0.4f;
	float RollCooldown = 0.25;
	float GroundTraceMultiplier = 5.f;
	float RollingGravityScale = 10.f;
	bool OrientRotationToMovement = true;
	bool OrientRotationToMovementWhenAiming = false;

protected:

	void StartRoll();

public:

	// Constructor
	USCM_Rolling() {}

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


/**
 * 
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_RollingLayer : public USCM_MovementStateMachine
{
	GENERATED_BODY()

protected:

public:

	// Constructor
	USCM_RollingLayer() { StateDisplayNames = { "None", "Rolling", "LowerLevel" }; }

	// Called when the state machine is initialized (after the main init)
	virtual void OnInitStateMachine_Implementation() override
	{
		// Registering States
		AddNewState((uint8)ESCM_RollingLayerStates::Rolling, USCM_Rolling::StaticClass());
		AddNewState((uint8)ESCM_RollingLayerStates::LowerLevel, USCM_LowerLayerState::StaticClass());

		//Alternatively you can use automatic Transition Registration:
		AutoTransitionRegistration(StateDisplayNames, "Condition_", "_");

		// Set Default State
		ForceCallStateTransition((uint8)ESCM_RollingLayerStates::LowerLevel);
	}

	// Called every time the state machine is updated (after the main update)
	virtual void OnUpdateStateMachine_Implementation(float DeltaTime) override
	{
		Super::OnUpdateStateMachine_Implementation(DeltaTime);
	}

	// Called every time the state is changed
	virtual void OnStateChangedInternal_Implementation(uint8 OldState, uint8 NewState) {}

	// Returns a pointer to the state that contains the lower level of the movement state machine stack
	virtual USSM_NestedStateMachineState* GetLowerLayerContainerState_Implementation() { return Cast<USSM_NestedStateMachineState>(GetState((uint8)ESCM_RollingLayerStates::LowerLevel)); }

	// Needed to register state-machine level movement parameters (for state transition conditions)
	virtual void OnSetupParameters_Implementation()
	{
		// Register control parameters here
		// ...

		// Dynamic gates
		GetScarletMovement()->RegisterDynamicGate("CanRoll", ESCM_DynamicGateRule::And, true);
		GetScarletMovement()->RegisterDynamicGate("StopRoll", ESCM_DynamicGateRule::Or, false);
		// ...

		// Timers
		// ...
	}

	UFUNCTION()
	void OnParameterValueChanged(const FName& ParameterName) {}

	UFUNCTION()
	void OnTimerIsOver(FName InTimerName) {}

	// Fired when this movement layer is entered
	virtual void OnLayerEntered_Implementation()
	{
		if (Condition_LowerLevel_Rolling())
			if (GetActiveState() != (uint8)ESCM_RollingLayerStates::Rolling)
			{
				GetState(GetActiveState())->ExitState();
				ActiveState = (uint8)ESCM_RollingLayerStates::Rolling;
			}
	}

	//  Fired when this movement layer is exited
	virtual void OnLayerExit_Implementation()
	{
		ForceCallStateTransition((uint8)ESCM_RollingLayerStates::LowerLevel);
		UpdateStateMachine(0.f);
	}

	// TRANSITION CONDITIONS
	// Must be a UFUNCTION, will not work otherwise, and no, it will not crash, you will just be stuck there with no real signs of errors, so yeah

	// Condition for transition from state One to state Two
	UFUNCTION()
	bool Condition_LowerLevel_Rolling() { return	GetScarletMovement()->GetBoolInputValue("Roll")
												 && GetScarletMovement()->GetDynamicGateValue("CanRoll")
												 && GetScarletMovement()->GetRawMovementInputVector() != FVector::ZeroVector; }

	UFUNCTION()
	bool Condition_Rolling_LowerLevel() { return GetScarletMovement()->GetDynamicGateValue("StopRoll"); }

	// ...

};
