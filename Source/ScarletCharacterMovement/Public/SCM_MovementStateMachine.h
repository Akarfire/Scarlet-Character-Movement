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

	// CALLED BY ScarletMovementComponent. Initializes ScarletMovement value
	void MOVEMENTSTATEMACHINE_SetScarletMovement(UScarletMovementComponent* InScarletMovementComponent) { ScarletMovement = InScarletMovementComponent; }

	// Returns a pointer to the ScarletMovementComponent that owns this state machine
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|MovementStateMachine")
	UScarletMovementComponent* GetScarletMovement() { return ScarletMovement; }

	// Called every time the state machine is updated (after the main update) (to be overriden)
	virtual void OnUpdateStateMachine_Implementation(float DeltaTime);

	// Returns a pointer to the Character Movement Component that is controlled by this Scarlet Movement State Machine
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|MovementStateMachine")
	class UCharacterMovementComponent* GetCharacterMovement() { return ScarletMovement->GetCharacterMovementComponent(); }
};
