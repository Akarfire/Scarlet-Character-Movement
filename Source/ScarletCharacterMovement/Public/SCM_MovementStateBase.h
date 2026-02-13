// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSM_StateBase.h"
#include "SCM_MovementStateMachine.h"
#include "SCM_MovementStateBase.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_MovementStateBase : public USSM_StateBase
{
	GENERATED_BODY()
	
protected:

	// A pointer to the ScarletMovementComponent that owns this state machine
	UScarletMovementComponent* ScarletMovement = nullptr;

public:

	// Must NEVER be called manually. Fires off after the main STATEMACHINE_OnSetStateMachine function has been called. Is used for implementing custom State base classes, derived from this one
	virtual void STATEMACHINE_OnSetStateMachine_Implementation() override;

	// CALLED BY THE SCARLET MOVEMENT : Should contain logic for registering parameters
	UFUNCTION(BlueprintNativeEvent, Category = "ScarletMovement|MovementStateMachine|State")
	void SetupParameters();
	virtual void SetupParameters_Implementation() {}

	// Returns a pointer to the ScarletMovementComponent that owns this state machine
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|MovementStateMachine|State")
	UScarletMovementComponent* GetScarletMovement() { return ScarletMovement; }

	// Returns a pointer to the Character Movement Component that is controlled by this Scarlet Movement State Machine
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|MovementStateMachine|State")
	class UCharacterMovementComponent* GetCharacterMovement() { return ScarletMovement->GetCharacterMovementComponent(); }
};
