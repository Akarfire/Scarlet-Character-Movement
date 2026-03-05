// Fill out your copyright notice in the Description page of Project Settings.


#include "SCM_AirMovement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"


// FALLING

// Registering parameters
void USCM_Falling::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	SM->RegisterFloatParameter("AirControl", AirControl, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("MovementInputIterpolationSpeed_Falling", MovementInputIterpolationSpeed, true, this, "OnParameterValueChanged");

	SM->RegisterBoolParameter("OrientRotationToMovement_Falling", OrientRotationToMovement, true, this, "OnParameterValueChanged");
	SM->RegisterBoolParameter("OrientRotationToMovementWhenAiming_Falling", OrientRotationToMovementWhenAiming, true, this, "OnParameterValueChanged");
}

// Called when any parameter value is changed
void USCM_Falling::OnParameterValueChanged(const FName& ParameterName)
{
	if (ParameterName == "AirControl")
		AirControl = GetScarletMovement()->GetFloatParameterValue("AirControl");

	else if (ParameterName == "MovementInputIterpolationSpeed_Falling")
		MovementInputIterpolationSpeed = GetScarletMovement()->GetFloatParameterValue("MovementInputIterpolationSpeed_Falling");

	else if (ParameterName == "OrientRotationToMovement_Falling")
		OrientRotationToMovement = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovement_Falling");

	else if (ParameterName == "OrientRotationToMovementWhenAiming_Falling")
		OrientRotationToMovementWhenAiming = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovementWhenAiming_Falling");

	// If the state is active
	if (GetScarletMovement()->GetActiveMovementState() == this)
	{
		GetCharacterMovement()->AirControl = AirControl;
		GetScarletMovement()->MovementInputInterpolationSpeed = MovementInputIterpolationSpeed;
	}
}

void USCM_Falling::EnterState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	CM->SetMovementMode(EMovementMode::MOVE_Walking);
	CM->AirControl = AirControl;

	GetScarletMovement()->MovementInputInterpolationSpeed = MovementInputIterpolationSpeed;
}

void USCM_Falling::ExitState_Implementation()
{
}

void USCM_Falling::UpdateState_Implementation(float DeltaTime)
{
	FVector MovementVector = GetScarletMovement()->GetMovementInputVector();
	GetCharacterMovement()->AddInputVector(MovementVector);

	// Orient Rotation To Movement
	bool IsAiming = GetScarletMovement()->GetBoolInputValue("Aim");

	GetCharacterMovement()->bOrientRotationToMovement = IsAiming ? OrientRotationToMovementWhenAiming : OrientRotationToMovement;
	GetScarletMovement()->GetCharacterMovementComponent()->GetCharacterOwner()->bUseControllerRotationYaw = !GetCharacterMovement()->bOrientRotationToMovement;
}


// JUMPING

// Registering parameters
void USCM_Jumping::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	SM->RegisterFloatParameter("JumpZVelocity", JumpZVelocity, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("AirControl_Jumping", AirControl, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("GroundTraceMultiplier_Jumping", GroundTraceMultiplier, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("HorizontalVelocityBoostFraction_Jumping", HorizontalVelocityBoostFraction, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("MovementInputIterpolationSpeed_Jumping", MovementInputIterpolationSpeed, true, this, "OnParameterValueChanged");

	SM->RegisterBoolParameter("OrientRotationToMovement_Jumping", OrientRotationToMovement, true, this, "OnParameterValueChanged");
	SM->RegisterBoolParameter("OrientRotationToMovementWhenAiming_Jumping", OrientRotationToMovementWhenAiming, true, this, "OnParameterValueChanged");

	// Control parameter
	GetScarletMovement()->RegisterFloatParameter("MaxJumpBoostTime", MaxJumpBoostTime, true, this, "OnParameterValueChanged");
}

// Called when any parameter value is changed
void USCM_Jumping::OnParameterValueChanged(const FName& ParameterName)
{
	if (ParameterName == "JumpZVelocity")
		JumpZVelocity = GetScarletMovement()->GetFloatParameterValue("JumpZVelocity");

	else if (ParameterName == "AirControl_Jumping")
		AirControl = GetScarletMovement()->GetFloatParameterValue("AirControl_Jumping");

	else if (ParameterName == "GroundTraceMultiplier_Jumping")
	{
		float OldGroundTraceMultiplier = GroundTraceMultiplier;
		GroundTraceMultiplier = GetScarletMovement()->GetFloatParameterValue("GroundTraceMultiplier_Jumping");
		if (GetStateMachine()->GetActiveState() == GetStateID())
		{
			GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") / OldGroundTraceMultiplier);
			if (GroundTraceMultiplier == 0.f)
				CachedGroundTraceDistance = GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance");
			
			GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") * GroundTraceMultiplier);
		}
	}

	else if (ParameterName == "HorizontalVelocityBoostFraction_Jumping")
		HorizontalVelocityBoostFraction = GetScarletMovement()->GetFloatParameterValue("HorizontalVelocityBoostFraction_Jumping");

	else if (ParameterName == "MovementInputIterpolationSpeed_Jumping")
		MovementInputIterpolationSpeed = GetScarletMovement()->GetFloatParameterValue("MovementInputIterpolationSpeed_Jumping");

	else if (ParameterName == "OrientRotationToMovement_Jumping")
		OrientRotationToMovement = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovement_Jumping");

	else if (ParameterName == "OrientRotationToMovementWhenAiming_Jumping")
		OrientRotationToMovementWhenAiming = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovementWhenAiming_Jumping");

	// Control parameter
	else if (ParameterName == "MaxJumpBoostTime")
		MaxJumpBoostTime = GetScarletMovement()->GetFloatParameterValue("MaxJumpBoostTime");

	// If the state is active
	if (GetScarletMovement()->GetActiveMovementState() == this)
	{
		GetCharacterMovement()->AirControl = AirControl;
		GetScarletMovement()->MovementInputInterpolationSpeed = MovementInputIterpolationSpeed;
	}
}

void USCM_Jumping::EnterState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	CM->SetMovementMode(EMovementMode::MOVE_Walking);
	CM->AirControl = AirControl;
	CM->JumpZVelocity = JumpZVelocity;

	GetScarletMovement()->MovementInputInterpolationSpeed = MovementInputIterpolationSpeed;

	// Ground trace distance
	if (GroundTraceMultiplier == 0.f)
		CachedGroundTraceDistance = GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance");
	GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") * GroundTraceMultiplier);

	// Initial jump
	FVector VerticalVelocity = GetCharacterMovement()->GetCharacterOwner()->GetActorUpVector() * JumpZVelocity;
	GetCharacterMovement()->GetCharacterOwner()->LaunchCharacter(VerticalVelocity, false, false);

}

void USCM_Jumping::ExitState_Implementation()
{
	// Ground trace distance
	if (GroundTraceMultiplier == 0.f)
		GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", CachedGroundTraceDistance);
	else
		GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") / GroundTraceMultiplier);
}

void USCM_Jumping::UpdateState_Implementation(float DeltaTime)
{
	FVector MovementVector = GetScarletMovement()->GetMovementInputVector();
	GetCharacterMovement()->AddInputVector(MovementVector);

	// Orient Rotation To Movement
	bool IsAiming = GetScarletMovement()->GetBoolInputValue("Aim");

	GetCharacterMovement()->bOrientRotationToMovement = IsAiming ? OrientRotationToMovementWhenAiming : OrientRotationToMovement;
	GetScarletMovement()->GetCharacterMovementComponent()->GetCharacterOwner()->bUseControllerRotationYaw = !GetCharacterMovement()->bOrientRotationToMovement;

	// Horizontal boost
	FVector HorizontalVelocity = GetCharacterMovement()->GetCharacterOwner()->GetVelocity() * (FVector::OneVector - GetCharacterMovement()->GetCharacterOwner()->GetActorUpVector());
	FVector VerticalVelocity = GetCharacterMovement()->GetCharacterOwner()->GetActorUpVector() * JumpZVelocity;

	float VelocityMultiplier = 1 / MaxJumpBoostTime * DeltaTime;
	
	GetCharacterMovement()->GetCharacterOwner()->LaunchCharacter((HorizontalVelocity  * HorizontalVelocityBoostFraction + VerticalVelocity) * VelocityMultiplier, false, false);

	// Actual jump
	//GetCharacterMovement()->GetCharacterOwner()->Jump();
}



// STATE MACHINE

// Casts a trace to find the ground, returns true if ground was found
bool USCM_AirMovement::GroundTrace()
{
	if (GroundTraceDistance == 0.f) return false;

	auto* Capsule = GetCharacterMovement()->GetCharacterOwner()->GetCapsuleComponent();

	FVector Start = GetScarletMovement()->GetOwner()->GetActorLocation();
	FVector End = Start - FVector::UpVector * GroundTraceDistance;
	FHitResult Hit;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetScarletMovement()->GetOwner());

	return GetScarletMovement()->GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, Capsule->GetCollisionShape(), CollisionQueryParams);
}
