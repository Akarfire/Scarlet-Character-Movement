// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCM_MovementStateMachine.h"
#include "SCM_MovementStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "SCM_GroundMovement.generated.h"

/*
 * Enum for the States of the State Machine
 * First value must always be None, as its considered as non-existance by the state machine
*/
UENUM(BlueprintType)
enum class ESCM_GroundMovementStates : uint8
{
	None UMETA(DisplayName = "None"),
	Walking UMETA(DisplayName = "Walking"),
	Running UMETA(DisplayName = "Running"),
	Crouching UMETA(DisplayName = "Crouching"),
	Sliding UMETA(DisplayName = "Sliding")
};


// STATES

/**
 * Walking state
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_Walking : public USCM_MovementStateBase
{
	GENERATED_BODY()
	
	// Parameters

	float WalkingSpeed = 400.0;
	float MovementInputIterpolationSpeed = 5.f;
	bool OrientRotationToMovement = true;
	bool OrientRotationToMovementWhenAiming = false;

public:

	// Constructor
	USCM_Walking() {}

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
 * Running state
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_Running : public USCM_MovementStateBase
{
	GENERATED_BODY()

	// Parameters

	float RunningSpeed = 700.0;
	float MovementInputIterpolationSpeed = 2.5f;
	bool OrientRotationToMovement = true;
	bool OrientRotationToMovementWhenAiming = true;

public:

	// Constructor
	USCM_Running() {}

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
 * Crouching state
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_Crouching : public USCM_MovementStateBase
{
	GENERATED_BODY()

	// Parameters

	float CapsuleHeightMultiplier = 0.5f;
	float CrouchWalkingSpeed = 250.0;
	float MovementInputIterpolationSpeed = 7.5f;
	bool OrientRotationToMovement = true;
	bool OrientRotationToMovementWhenAiming = false;

public:

	// Constructor
	USCM_Crouching() {}

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
 * Sliding state
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_Sliding : public USCM_MovementStateBase
{
	GENERATED_BODY()

	// Parameters

	float CapsuleHeightMultiplier = 0.5f;
	float SlideBoost = 1250.f;
	float GroundFriction = 0.0;
	bool OrientRotationToMovement = true;
	bool OrientRotationToMovementWhenAiming = false;

	// Cache
	float PreviousGroundFriction = 0.0;

public:

	// Constructor
	USCM_Sliding() {}

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
class SCARLETCHARACTERMOVEMENT_API USCM_GroundMovement : public USCM_MovementStateMachine
{
	GENERATED_BODY()
	
protected:
	float SlidingThresholdSpeed = 400.f;

public:

	// Constructor
	USCM_GroundMovement() { StateDisplayNames = { "None", "Walking", "Running", "Crouching", "Sliding" }; }

	// Called when the state machine is initialized (after the main init)
	virtual void OnInitStateMachine_Implementation() override
	{
		// Registering States
		AddNewState((uint8)ESCM_GroundMovementStates::Walking, USCM_Walking::StaticClass());
		AddNewState((uint8)ESCM_GroundMovementStates::Running, USCM_Running::StaticClass());
		AddNewState((uint8)ESCM_GroundMovementStates::Crouching, USCM_Crouching::StaticClass());
		AddNewState((uint8)ESCM_GroundMovementStates::Sliding, USCM_Sliding::StaticClass());

		//Alternatively you can use automatic Transition Registration:
		AutoTransitionRegistration(StateDisplayNames, "Condition_", "_");

		// Set Default State
		ForceCallStateTransition((uint8)ESCM_GroundMovementStates::Walking);
	}

	// Called every time the state machine is updated (after the main update)
	virtual void OnUpdateStateMachine_Implementation(float DeltaTime) override { Super::OnUpdateStateMachine_Implementation(DeltaTime); }

	// Needed to register state-machine level movement parameters (for state transition conditions)
	virtual void OnSetupParameters_Implementation() 
	{
		GetScarletMovement()->RegisterFloatParameter("SlidingThresholdSpeed", SlidingThresholdSpeed, true, this, "OnParameterValueChanged");
	}

	UFUNCTION()
	void OnParameterValueChanged(const FName& ParameterName)
	{
		if (ParameterName == "SlidingThresholdSpeed")
			SlidingThresholdSpeed = GetScarletMovement()->GetFloatParameterValue("SlidingThresholdSpeed");
	}

	// TRANSITION CONDITIONS
	// Must be a UFUNCTION, will not work otherwise, and no, it will not crash, you will just be stuck there with no real signs of errors, so yeah

	// Condition for transition from state One to state Two
	UFUNCTION()
	bool Condition_Walking_Running() { return GetScarletMovement()->GetBoolInputValue("Run"); }

	UFUNCTION()
	bool Condition_Walking_Crouching() { return	GetScarletMovement()->GetBoolInputValue("Crouch") 
												&& GetCharacterMovement()->GetCharacterOwner()->GetVelocity().Size() < SlidingThresholdSpeed; }

	UFUNCTION()
	bool Condition_Walking_Sliding() { return GetScarletMovement()->GetBoolInputValue("Crouch") 
											  && GetCharacterMovement()->GetCharacterOwner()->GetVelocity().Size() >= SlidingThresholdSpeed; }


	UFUNCTION()
	bool Condition_Running_Walking() { return !GetScarletMovement()->GetBoolInputValue("Run"); }

	UFUNCTION()
	bool Condition_Running_Sliding() { return GetScarletMovement()->GetBoolInputValue("Crouch") 
											  && GetCharacterMovement()->GetCharacterOwner()->GetVelocity().Size() >= SlidingThresholdSpeed; }


	UFUNCTION()
	bool Condition_Crouching_Walking() { return !GetScarletMovement()->GetBoolInputValue("Crouch"); }

	UFUNCTION()
	bool Condition_Crouching_Sliding() { return GetCharacterMovement()->GetCharacterOwner()->GetVelocity().Size() >= SlidingThresholdSpeed; }


	UFUNCTION()
	bool Condition_Sliding_Walking() { return !GetScarletMovement()->GetBoolInputValue("Crouch") 
											  && GetCharacterMovement()->GetCharacterOwner()->GetVelocity().Size() < SlidingThresholdSpeed; }

	UFUNCTION()
	bool Condition_Sliding_Crouching() { return	GetScarletMovement()->GetBoolInputValue("Crouch") 
												&& GetCharacterMovement()->GetCharacterOwner()->GetVelocity().Size() < SlidingThresholdSpeed; }
	//...
};
