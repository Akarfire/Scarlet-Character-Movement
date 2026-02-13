// Fill out your copyright notice in the Description page of Project Settings.


#include "SCM_GroundMovement.h"
#include "GameFramework/CharacterMovementComponent.h"

// WALKING

// Registering parameters
void USCM_Walking::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	if (!SM->IsFloatParameterValid("WalkingSpeed"))
		SM->SetFloatParameterValue("WalkingSpeed", WalkingSpeed);
	SM->SubscribeToParameter("WalkingSpeed", this, "OnParameterValueChanged");

	if (!SM->IsFloatParameterValid("MovementInputIterpolationSpeed"))
		SM->SetFloatParameterValue("MovementInputIterpolationSpeed", MovementInputIterpolationSpeed);
	SM->SubscribeToParameter("MovementInputIterpolationSpeed", this, "OnParameterValueChanged");

	if (!SM->IsBoolParameterValid("OrientRotationToMovement"))
		SM->SetBoolParameterValue("OrientRotationToMovement", OrientRotationToMovement);
	SM->SubscribeToParameter("OrientRotationToMovement", this, "OnParameterValueChanged");

	if (!SM->IsBoolParameterValid("OrientRotationToMovementWhenAiming"))
		SM->SetBoolParameterValue("OrientRotationToMovementWhenAiming", OrientRotationToMovementWhenAiming);
	SM->SubscribeToParameter("OrientRotationToMovementWhenAiming", this, "OnParameterValueChanged");
}

// Called when any parameter value is changed
void USCM_Walking::OnParameterValueChanged(const FName& ParameterName)
{
	if (ParameterName == "WalkingSpeed")
		WalkingSpeed = GetScarletMovement()->GetFloatParameterValue("WalkingSpeed");

	else if (ParameterName == "MovementInputIterpolationSpeed")
		MovementInputIterpolationSpeed = GetScarletMovement()->GetFloatParameterValue("MovementInputIterpolationSpeed");

	else if (ParameterName == "OrientRotationToMovement")
		OrientRotationToMovement = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovement");

	else if (ParameterName == "OrientRotationToMovementWhenAiming")
		OrientRotationToMovementWhenAiming = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovementWhenAiming");

	// If the state is active
	if (GetStateMachine()->GetActiveState() == StateID)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
		GetScarletMovement()->MovementInputInterpolationSpeed = MovementInputIterpolationSpeed;
	}
}

void USCM_Walking::EnterState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	CM->SetMovementMode(EMovementMode::MOVE_Walking);
	CM->MaxWalkSpeed = WalkingSpeed;
	
	GetScarletMovement()->MovementInputInterpolationSpeed = MovementInputIterpolationSpeed;
}

void USCM_Walking::ExitState_Implementation()
{
}

void USCM_Walking::UpdateState_Implementation(float DeltaTime)
{
	FVector MovementVector = GetScarletMovement()->GetMovementInputVector();
	GetCharacterMovement()->AddInputVector(MovementVector);

	// Orient Rotation To Movement
	bool IsAiming = GetScarletMovement()->GetBoolInputValue("Aim");

	GetCharacterMovement()->bOrientRotationToMovement = IsAiming ? OrientRotationToMovementWhenAiming : OrientRotationToMovement;
}



// RUNNING

// Registering parameters
void USCM_Running::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	if (!SM->IsFloatParameterValid("RunningSpeed"))
		SM->SetFloatParameterValue("RunningSpeed", RunningSpeed);
	SM->SubscribeToParameter("RunningSpeed", this, "OnParameterValueChanged");

	if (!SM->IsFloatParameterValid("MovementInputIterpolationSpeed"))
		SM->SetFloatParameterValue("MovementInputIterpolationSpeed", MovementInputIterpolationSpeed);
	SM->SubscribeToParameter("MovementInputIterpolationSpeed", this, "OnParameterValueChanged");

	if (!SM->IsBoolParameterValid("OrientRotationToMovementWhenRunning"))
		SM->SetBoolParameterValue("OrientRotationToMovementWhenRunning", OrientRotationToMovementWhenRunning);
	SM->SubscribeToParameter("OrientRotationToMovementWhenRunning", this, "OnParameterValueChanged");
}

// Called when any parameter value is changed
void USCM_Running::OnParameterValueChanged(const FName& ParameterName)
{
	if (ParameterName == "RunningSpeed")
		RunningSpeed = GetScarletMovement()->GetFloatParameterValue("RunningSpeed");

	else if (ParameterName == "MovementInputIterpolationSpeed")
		MovementInputIterpolationSpeed = GetScarletMovement()->GetFloatParameterValue("MovementInputIterpolationSpeed");

	else if (ParameterName == "OrientRotationToMovementWhenRunning")
		OrientRotationToMovementWhenRunning = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovementWhenRunning");

	// If the state is active
	if (GetStateMachine()->GetActiveState() == StateID)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		GetScarletMovement()->MovementInputInterpolationSpeed = MovementInputIterpolationSpeed;
	}
}

void USCM_Running::EnterState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	CM->SetMovementMode(EMovementMode::MOVE_Walking);
	CM->MaxWalkSpeed = RunningSpeed;

	GetScarletMovement()->MovementInputInterpolationSpeed = MovementInputIterpolationSpeed;
}

void USCM_Running::ExitState_Implementation()
{
}

void USCM_Running::UpdateState_Implementation(float DeltaTime)
{
	FVector MovementVector = GetScarletMovement()->GetMovementInputVector();
	GetCharacterMovement()->AddInputVector(MovementVector);

	// Orient Rotation To Movement
	GetCharacterMovement()->bOrientRotationToMovement = OrientRotationToMovementWhenRunning;
}
