// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSM_StateMachine.h"
#include "ScarletMovementComponent.h"
#include "SCM_MovementStateMachine.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_MovementStateMachine : public USSM_StateMachine
{
	GENERATED_BODY()
	
protected:

	// A pointer to the ScarletMovementComponent that owns this state machine
	UScarletMovementComponent* ScarletMovement = nullptr;

public:

	// Called every time the state machine is updated (after the main update) (to be overriden)
	virtual void OnUpdateStateMachine_Implementation(float DeltaTime);

	// CALLED BY ScarletMovementComponent. Initializes ScarletMovement value
	void MOVEMENTSTATEMACHINE_SetScarletMovement(UScarletMovementComponent* InScarletMovementComponent) { ScarletMovement = InScarletMovementComponent; }

	// CALLED BY ScarletMovementComponent. Call SetupParameters on the states and triggers OnSetupParameters
	void SetupParameters();

	// Needed to register state-machine level movement parameters (for state transition conditions)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnSetupParameters();
	virtual void OnSetupParameters_Implementation() {}

	// Returns a pointer to the ScarletMovementComponent that owns this state machine
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|MovementStateMachine")
	UScarletMovementComponent* GetScarletMovement() { return ScarletMovement; }

	// Returns a pointer to the Character Movement Component that is controlled by this Scarlet Movement State Machine
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|MovementStateMachine")
	class UCharacterMovementComponent* GetCharacterMovement() { return ScarletMovement->GetCharacterMovementComponent(); }
};
