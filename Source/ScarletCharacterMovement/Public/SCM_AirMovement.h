// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCM_MovementStateMachine.h"
#include "SCM_MovementStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "SCM_LowerLevelState.h"
#include "SCM_StateMachineInterface.h"
#include "SCM_AirMovement.generated.h"

UENUM(BlueprintType)
enum class ESCM_AirMovementStates : uint8
{
	None UMETA(DisplayName = "None"),
	Falling UMETA(DisplayName = "Falling"),
	Jumping UMETA(DisplayName = "Jumping"),
	LowerLevel UMETA(DisplayName = "LowerLevel")
};

// STATES

/**
 * Falling state
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_Falling : public USCM_MovementStateBase
{
	GENERATED_BODY()

	// Parameters

	float AirControl = 0.2f;
	float MovementInputIterpolationSpeed = 1.f;
	bool OrientRotationToMovement = true;
	bool OrientRotationToMovementWhenAiming = false;

public:

	// Constructor
	USCM_Falling() {}

	// Called when any parameter value is changed
	UFUNCTION()
	void OnParameterValueChanged(const FName& ParameterName);

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
 * Jumping state
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_Jumping : public USCM_MovementStateBase
{
	GENERATED_BODY()

	// Parameters

	float JumpZVelocity = 750.f;
	float AirControl = 0.4f;
	float HorizontalVelocityBoostFraction = 0.5f;
	float MovementInputIterpolationSpeed = 1.f;
	bool OrientRotationToMovement = true;
	bool OrientRotationToMovementWhenAiming = false;

public:

	// Constructor
	USCM_Jumping() {}

	// Called when any parameter value is changed
	UFUNCTION()
	void OnParameterValueChanged(const FName& ParameterName);

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
class SCARLETCHARACTERMOVEMENT_API USCM_AirMovement : public USCM_MovementStateMachine
{
	GENERATED_BODY()

protected:

	bool IsGrounded = true;

	// Casts a trace to find the ground, returns true if ground was found
	bool GroundTrace();

	float JumpTime = 0.f;

	// Parameters
	float GroundTraceDistance = 10.f;
	float MaxJumpBoostTime = 0.25f;
	float JumpCoolDown = 0.2f;

public:

	// Constructor
	USCM_AirMovement() { StateDisplayNames = { "None", "Falling", "Jumping", "LowerLevel" }; }

	// Called when the state machine is initialized (after the main init)
	virtual void OnInitStateMachine_Implementation() override
	{
		// Registering States
		AddNewState((uint8)ESCM_AirMovementStates::Falling, USCM_Falling::StaticClass());
		AddNewState((uint8)ESCM_AirMovementStates::Jumping, USCM_Jumping::StaticClass());
		AddNewState((uint8)ESCM_AirMovementStates::LowerLevel, USCM_LowerLayerState::StaticClass());

		//Alternatively you can use automatic Transition Registration:
		AutoTransitionRegistration(StateDisplayNames, "Condition_", "_");

		// Set Default State
		ForceCallStateTransition((uint8)ESCM_AirMovementStates::LowerLevel);
	}

	// Called every time the state machine is updated (after the main update)
	virtual void OnUpdateStateMachine_Implementation(float DeltaTime) override 
	{ 
		Super::OnUpdateStateMachine_Implementation(DeltaTime);

		IsGrounded = GroundTrace();

		if (GetActiveState() == (uint8)ESCM_AirMovementStates::Jumping)
			JumpTime += DeltaTime;		
	}

	// Called every time the state is changed
	virtual void OnStateChangedInternal_Implementation(uint8 OldState, uint8 NewState) 
	{
		if (NewState == (uint8)ESCM_AirMovementStates::LowerLevel)
		{
			GetScarletMovement()->SetDynamicGateNamedValue("CanJump", "Cooldown", false);

			GetScarletMovement()->GetTimerController()->ResetTimer("JumpCooldown");
			GetScarletMovement()->GetTimerController()->StartTimer("JumpCooldown");

			JumpTime = 0.f;
		}
	}

	// Returns a pointer to the state that contains the lower level of the movement state machine stack
	virtual USSM_NestedStateMachineState* GetLowerLayerContainerState_Implementation() { return Cast<USSM_NestedStateMachineState>(GetState((uint8)ESCM_AirMovementStates::LowerLevel)); }

	// Needed to register state-machine level movement parameters (for state transition conditions)
	virtual void OnSetupParameters_Implementation()
	{
		// Register control parameters here
		GetScarletMovement()->RegisterFloatParameter("GroundTraceDistance", GroundTraceDistance, true, this, "OnParameterValueChanged");
		GetScarletMovement()->RegisterFloatParameter("MaxJumpBoostTime", MaxJumpBoostTime, true, this, "OnParameterValueChanged");
		GetScarletMovement()->RegisterFloatParameter("JumpCoolDown", JumpCoolDown, true, this, "OnParameterValueChanged");
		// ...

		// Dynamic gates
		GetScarletMovement()->RegisterDynamicGate("CanJump", ESCM_DynamicGateRule::And, true);

		// Timers
		GetScarletMovement()->GetTimerController()->CreateTimer("JumpCooldown", JumpCoolDown, false, false);
		GetScarletMovement()->GetTimerController()->SubscribeToTimer("JumpCooldown", this, "OnJumpCooldownIsOver");
	}

	UFUNCTION()
	void OnParameterValueChanged(const FName& ParameterName)
	{
		// Update control parameter values here
		if (ParameterName == "GroundTraceDistance")
			GroundTraceDistance = GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance");

		else if (ParameterName == "MaxJumpBoostTime")
			MaxJumpBoostTime = GetScarletMovement()->GetFloatParameterValue("MaxJumpBoostTime");

		else if (ParameterName == "JumpCoolDown")
		{
			JumpCoolDown = GetScarletMovement()->GetFloatParameterValue("JumpCoolDown");
			GetScarletMovement()->GetTimerController()->ChangeTimerLength("JumpCoolDown", JumpCoolDown);
		}
		// ...
	}

	UFUNCTION()
	void OnJumpCooldownIsOver(FName InTimerName) { GetScarletMovement()->SetDynamicGateNamedValue("CanJump", "Cooldown", true); }

	// TRANSITION CONDITIONS
	// Must be a UFUNCTION, will not work otherwise, and no, it will not crash, you will just be stuck there with no real signs of errors, so yeah

	// Condition for transition from state One to state Two
	UFUNCTION()
	bool Condition_LowerLevel_Falling() { return !IsGrounded; }

	UFUNCTION()
	bool Condition_LowerLevel_Jumping() { return GetScarletMovement()->GetDynamicGateValue("CanJump") && GetScarletMovement()->GetBoolInputValue("Jump"); }


	UFUNCTION()
	bool Condition_Falling_LowerLevel() { return IsGrounded; }


	UFUNCTION()
	bool Condition_Jumping_Falling() { return (!GetScarletMovement()->GetBoolInputValue("Jump") || JumpTime >= MaxJumpBoostTime) && !IsGrounded; }

	UFUNCTION()
	bool Condition_Jumping_LowerLevel() { return (!GetScarletMovement()->GetBoolInputValue("Jump") || JumpTime >= MaxJumpBoostTime) && IsGrounded; }

	//...
};
