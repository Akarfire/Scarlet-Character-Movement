// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSM_NestedStateMachineState.h"
#include "SCM_LowerLevelState.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API USCM_LowerLayerState : public USSM_NestedStateMachineState
{
	GENERATED_BODY()
	

	// Called when the state is made active
	virtual void EnterState_Implementation() override;

	// Called once the active state is changed to a different one
	virtual void ExitState_Implementation() override;
};
