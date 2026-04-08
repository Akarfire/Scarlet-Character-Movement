// Fill out your copyright notice in the Description page of Project Settings.


#include "SCM_RollingLayer.h"


// ROLLING

// Registering parameters
void USCM_Rolling::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	SM->RegisterFloatParameter("RollBoost", RollBoost, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("RollDelay", RollDelay, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("RollDuration", RollDuration, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("RollCooldown", RollCooldown, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("RollingHorizontalVelocityConservation", HorizontalVelocityConservation, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("GroundTraceMultiplier_Rolling", GroundTraceMultiplier, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("RollingGravityScale", RollingGravityScale, true, this, "OnParameterValueChanged");

	SM->RegisterBoolParameter("OrientRotationToMovement_Rolling", OrientRotationToMovement, true, this, "OnParameterValueChanged");
	SM->RegisterBoolParameter("OrientRotationToMovementWhenAiming_Rolling", OrientRotationToMovementWhenAiming, true, this, "OnParameterValueChanged");

	// Timers
	USTT_TimerController* TC = SM->GetTimerController();

	TC->CreateTimer("RollDelay", RollDelay, false, false);
	TC->SubscribeToTimer("RollDelay", this, "OnTimerIsOver");

	TC->CreateTimer("RollDuration", RollDuration, false, false);
	TC->SubscribeToTimer("RollDuration", this, "OnTimerIsOver");

	TC->CreateTimer("RollCooldown", RollCooldown, false, false);
	TC->SubscribeToTimer("RollCooldown", this, "OnTimerIsOver");

	// Dynamic gates
	if (SM->IsDynamicGateValid("CanJump"))
		SM->SetDynamicGateNamedValue("CanJump", "NotRolling", true);

	if (SM->IsDynamicGateValid("CanRoll"))
		SM->SetDynamicGateNamedValue("CanRoll", "RollCooldown", true);

	if (SM->IsDynamicGateValid("StopRoll"))
		SM->SetDynamicGateNamedValue("StopRoll", "RollDuration", false);
}

// Called when any parameter value is changed
void USCM_Rolling::OnParameterValueChanged(const FName& ParameterName)
{
	if (ParameterName == "RollBoost")
		RollBoost = GetScarletMovement()->GetFloatParameterValue("RollBoost");

	else if (ParameterName == "RollingHorizontalVelocityConservation")
		HorizontalVelocityConservation = GetScarletMovement()->GetFloatParameterValue("RollingHorizontalVelocityConservation");

	else if (ParameterName == "RollingGravityScale")
		RollingGravityScale = GetScarletMovement()->GetFloatParameterValue("RollingGravityScale");

	else if (ParameterName == "GroundTraceMultiplier_Rolling")
	{
		float OldGroundTraceMultiplier = GroundTraceMultiplier;
		GroundTraceMultiplier = GetScarletMovement()->GetFloatParameterValue("GroundTraceMultiplier_Rolling");
		if (GetStateMachine()->GetActiveState() == GetStateID())
		{
			GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") / OldGroundTraceMultiplier);
			GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") * GroundTraceMultiplier);
		}
	}

	else if (ParameterName == "RollDelay")
	{
		RollDelay = GetScarletMovement()->GetFloatParameterValue("RollDelay");
		GetScarletMovement()->GetTimerController()->ChangeTimerLength("RollDelay", RollDelay);
	}

	else if (ParameterName == "RollDuration")
	{
		RollDuration = GetScarletMovement()->GetFloatParameterValue("RollDuration");
		GetScarletMovement()->GetTimerController()->ChangeTimerLength("RollDuration", RollDuration);
	}

	else if (ParameterName == "RollCooldown")
	{
		RollCooldown = GetScarletMovement()->GetFloatParameterValue("RollCooldown");
		GetScarletMovement()->GetTimerController()->ChangeTimerLength("RollCooldown", RollCooldown);
	}

	else if (ParameterName == "OrientRotationToMovement_Rolling")
		OrientRotationToMovement = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovement_Rolling");

	else if (ParameterName == "OrientRotationToMovementWhenAiming_Rolling")
		OrientRotationToMovementWhenAiming = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovementWhenAiming_Rolling");
}

void USCM_Rolling::OnTimerIsOver(FName TimerName)
{
	if (TimerName == "RollDelay") { StartRoll(); }

	else if (TimerName == "RollDuration") 
	{
		if (GetScarletMovement()->IsDynamicGateValid("StopRoll"))
			GetScarletMovement()->SetDynamicGateNamedValue("StopRoll", "RollDuration", true);
	}

	else if (TimerName == "RollCooldown")
	{
		if (GetScarletMovement()->IsDynamicGateValid("CanRoll"))
			GetScarletMovement()->SetDynamicGateNamedValue("CanRoll", "RollCooldown", true);
	}

}

void USCM_Rolling::StartRoll()
{
	FVector InputDirection = CachedInputVector;

	FVector BoostVector = InputDirection * RollBoost;

	GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity * HorizontalVelocityConservation;
	GetCharacterMovement()->GetCharacterOwner()->LaunchCharacter(BoostVector, false, false);

	// Character rotation
	FRotator NewRotation = InputDirection.Rotation();
	GetCharacterMovement()->GetCharacterOwner()->SetActorRotation(NewRotation);

	// RollDuration -> ... Timer ... -> StopRoll -> ExitState()
	GetScarletMovement()->GetTimerController()->StartTimer("RollDuration");
}

void USCM_Rolling::EnterState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	CM->SetMovementMode(EMovementMode::MOVE_Walking);
	CachedGroundFriction = CM->GroundFriction;
	CM->GroundFriction = 0.f;
	CachedGravityScale = CM->GravityScale;
	CM->GravityScale = RollingGravityScale;

	// Dynamic gates
	if (GetScarletMovement()->IsDynamicGateValid("CanJump"))
		GetScarletMovement()->SetDynamicGateNamedValue("CanJump", "NotRolling", false);

	if (GetScarletMovement()->IsDynamicGateValid("StopRoll"))
		GetScarletMovement()->SetDynamicGateNamedValue("StopRoll", "RollDuration", false);

	// Ground trace distance
	GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") * GroundTraceMultiplier);

	// Caching input direction
	CachedInputVector = GetScarletMovement()->GetRawMovementInputVector();

	// Start rolling
	// RollDelay -> ... Timer ... -> StartRoll();
	GetScarletMovement()->GetTimerController()->StartTimer("RollDelay");
}

void USCM_Rolling::ExitState_Implementation()
{
	GetCharacterMovement()->GroundFriction = CachedGroundFriction;
	GetCharacterMovement()->GravityScale = CachedGravityScale;

	// Dynamic gates
	if (GetScarletMovement()->IsDynamicGateValid("CanJump"))
		GetScarletMovement()->SetDynamicGateNamedValue("CanJump", "NotRolling", true);

	if (GetScarletMovement()->IsDynamicGateValid("CanRoll"))
		GetScarletMovement()->SetDynamicGateNamedValue("CanRoll", "RollCooldown", false);

	// Ground trace distance
	GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") / GroundTraceMultiplier);

	GetScarletMovement()->GetTimerController()->StartTimer("RollCooldown");
}

void USCM_Rolling::UpdateState_Implementation(float DeltaTime)
{
	// Orient Rotation To Movement
	bool IsAiming = GetScarletMovement()->GetBoolInputValue("Aim");

	GetCharacterMovement()->bOrientRotationToMovement = IsAiming ? OrientRotationToMovementWhenAiming : OrientRotationToMovement;
	GetScarletMovement()->GetCharacterMovementComponent()->GetCharacterOwner()->bUseControllerRotationYaw = !GetCharacterMovement()->bOrientRotationToMovement;
}