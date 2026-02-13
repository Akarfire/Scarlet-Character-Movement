// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SSM_NestedStateMachineState.h"
#include "SCM_StateMachineInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USCM_StateMachineInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SCARLETCHARACTERMOVEMENT_API ISCM_StateMachineInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Returns a pointer to the state that contains the lower level of the movement state machine stack
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ScarletMovement|Layers")
	USSM_NestedStateMachineState* GetLowerLayerContainerState();
};
