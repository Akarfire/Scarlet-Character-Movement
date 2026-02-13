// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ScarletMovementFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETCHARACTERMOVEMENT_API UScarletMovementFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Standart movement input vector processing:
	//    - Normalizes input vector, thus fixing diagonal ovelapping
	//    - Keeps analog input range
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	static FVector ProcessMovementInputVector(const FVector& InputVector);
};
