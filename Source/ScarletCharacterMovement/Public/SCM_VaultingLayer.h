// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCM_MovementStateMachine.h"
#include "SCM_MovementStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "SCM_LowerLevelState.h"
#include "SCM_VaultingLayer.generated.h"

UENUM(BlueprintType)
enum class ESCM_VaultingLayerStates : uint8
{
	None UMETA(DisplayName = "None"),
	Vaulting UMETA(DisplayName = "Vaulting"),
	LowerLevel UMETA(DisplayName = "LowerLevel")
};

// STATES

/**
 * Vaulting state
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_Vaulting : public USCM_MovementStateBase
{
	GENERATED_BODY()

protected:

	FVector SnapInitialLocation = FVector::ZeroVector;
	FVector SnapTargetLocation = FVector::ZeroVector;
	float SnapInitialZRotation = 0.f;
	float SnapTargetZRotation = 0.f;

	// Parameters
	float VaultingSnapAnimationPlaySpeed = 2.f;
	float VaultingDuration = 0.3f;

public:

	// Set by the state machine
	FVector VaultLocation = FVector::ZeroVector;
	FVector WallNormal = FVector::ZeroVector;

	// Constructor
	USCM_Vaulting() {}

	// Called when any parameter value is changed
	UFUNCTION()
	void OnParameterValueChanged(const FName& ParameterName);

	UFUNCTION()
	void OnTimerIsOver(FName TimerName);

	UFUNCTION()
	void OnSnapAnimationUpdated(FName TimelineName, float CurrentTime);

	UFUNCTION()
	void OnSnapAnimationFinished(FName TimelineName);

	// Registering parameters
	virtual void SetupParameters_Implementation() override;

	// Called when the state is made active
	virtual void EnterState_Implementation() override;

	// Called once the active state is changed to a different one
	virtual void ExitState_Implementation() override;

	// Called every state machine update when the state is active
	virtual void UpdateState_Implementation(float DeltaTime) override;
};


// STATE MACHINE

/**
 * 
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_VaultingLayer : public USCM_MovementStateMachine
{
	GENERATED_BODY()
	
protected:

	// Parameters
	float WallTraceDistance = 150.f;
	float InWallExtention = 25.f;
	float MaxWallHeight = 250.f;
	float MinGroundLevelness = 0.75f;
	float GroundCheckTollerance = 5.f;

	FVector VaultLocation = FVector::ZeroVector;
	FVector WallNormal = FVector::ZeroVector;
	bool CheckCanVault();

public:

	// Constructor
	USCM_VaultingLayer() { StateDisplayNames = { "None", "Vaulting", "LowerLevel" }; }

	// Called when the state machine is initialized (after the main init)
	virtual void OnInitStateMachine_Implementation() override
	{
		// Registering States
		AddNewState((uint8)ESCM_VaultingLayerStates::Vaulting, USCM_Vaulting::StaticClass());
		AddNewState((uint8)ESCM_VaultingLayerStates::LowerLevel, USCM_LowerLayerState::StaticClass());

		//Alternatively you can use automatic Transition Registration:
		AutoTransitionRegistration(StateDisplayNames, "Condition_", "_");

		// Set Default State
		ForceCallStateTransition((uint8)ESCM_VaultingLayerStates::LowerLevel);
	}

	// Called every time the state machine is updated (after the main update)
	virtual void OnUpdateStateMachine_Implementation(float DeltaTime) override
	{
		Super::OnUpdateStateMachine_Implementation(DeltaTime);

		// Passing cached wall normal to the state
		USCM_Vaulting* VaultingState = Cast<USCM_Vaulting>(GetState((uint8)ESCM_VaultingLayerStates::Vaulting));
		VaultingState->VaultLocation = VaultLocation;
		VaultingState->WallNormal = WallNormal;
	}

	// Called every time the state is changed
	virtual void OnStateChangedInternal_Implementation(uint8 OldState, uint8 NewState) {}

	// Returns a pointer to the state that contains the lower level of the movement state machine stack
	virtual USSM_NestedStateMachineState* GetLowerLayerContainerState_Implementation() 
	{ return Cast<USSM_NestedStateMachineState>(GetState((uint8)ESCM_VaultingLayerStates::LowerLevel)); }

	// Needed to register state-machine level movement parameters (for state transition conditions)
	virtual void OnSetupParameters_Implementation()
	{
		UScarletMovementComponent* SM = GetScarletMovement();

		// Register control parameters here
		SM->RegisterFloatParameter("Vaulting_WallTraceDistance", WallTraceDistance, true, this, "OnParameterValueChanged");
		SM->RegisterFloatParameter("Vaulting_InWallExtention", InWallExtention, true, this, "OnParameterValueChanged");
		SM->RegisterFloatParameter("Vaulting_MaxWallHeight", MaxWallHeight, true, this, "OnParameterValueChanged");
		SM->RegisterFloatParameter("Vaulting_MinGroundLevelness", MinGroundLevelness, true, this, "OnParameterValueChanged");
		SM->RegisterFloatParameter("Vaulting_GroundCheckTollerance", GroundCheckTollerance, true, this, "OnParameterValueChanged");
		// ...

		// Dynamic gates
		GetScarletMovement()->RegisterDynamicGate("CanVault", ESCM_DynamicGateRule::And, true);
		GetScarletMovement()->RegisterDynamicGate("StopVault", ESCM_DynamicGateRule::Or, false);
		// ...

		// Timers
		// ...
	}

	UFUNCTION()
	void OnParameterValueChanged(const FName& ParameterName) 
	{
		if (ParameterName == "Vaulting_WallTraceDistance")
			WallTraceDistance = GetScarletMovement()->GetFloatParameterValue("Vaulting_WallTraceDistance");

		else if (ParameterName == "Vaulting_InWallExtention")
			InWallExtention = GetScarletMovement()->GetFloatParameterValue("Vaulting_InWallExtention");

		else if (ParameterName == "Vaulting_MaxWallHeight")
			MaxWallHeight = GetScarletMovement()->GetFloatParameterValue("Vaulting_MaxWallHeight");

		else if (ParameterName == "Vaulting_MinGroundLevelness")
			MinGroundLevelness = GetScarletMovement()->GetFloatParameterValue("Vaulting_MinGroundLevelness");

		else if (ParameterName == "Vaulting_GroundCheckTollerance")
			GroundCheckTollerance = GetScarletMovement()->GetFloatParameterValue("Vaulting_GroundCheckTollerance");
	}

	UFUNCTION()
	void OnTimerIsOver(FName InTimerName) {}

	// TRANSITION CONDITIONS
	// Must be a UFUNCTION, will not work otherwise, and no, it will not crash, you will just be stuck there with no real signs of errors, so yeah

	// Condition for transition from state One to state Two
	UFUNCTION()
	bool Condition_LowerLevel_Vaulting() {
		return	GetScarletMovement()->GetBoolInputValue("Vault")
				&& GetScarletMovement()->GetDynamicGateValue("CanVault")
				&& CheckCanVault();
	}

	UFUNCTION()
	bool Condition_Vaulting_LowerLevel() {
		return GetScarletMovement()->GetDynamicGateValue("StopVault");
	}

};
