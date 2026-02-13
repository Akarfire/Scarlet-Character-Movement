// Fill out your copyright notice in the Description page of Project Settings.


#include "ScarletMovementFunctionLibrary.h"

FVector UScarletMovementFunctionLibrary::ProcessMovementInputVector(const FVector& InputVector)
{
	FVector InputDirection = InputVector.GetSafeNormal();

	// Though process for the next formula

	// X = InputX * abs(Dot(InputDirection, Ox))
	// Y = InputY * abs(Dot(InputDirection, Oy))
	// Z = InputZ * abs(Dot(InputDirection, Oz))

	// Dot(InputDirection, Ox) = InputDirection.x * 1.0 + InputDirection.y * 0.0 + InputDirection.z * 0.0 = InputDirection.x
	// ...

	FVector OutVector = InputVector * InputDirection.GetAbs();

	return OutVector;
}
