// Fill out your copyright notice in the Description page of Project Settings.


#include "SCM_GroundMovement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

// WALKING

// Registering parameters
void USCM_Walking::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	SM->RegisterFloatParameter("WalkingSpeed", WalkingSpeed, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("MovementInputIterpolationSpeed", MovementInputIterpolationSpeed, true, this, "OnParameterValueChanged");

	SM->RegisterBoolParameter("OrientRotationToMovement", OrientRotationToMovement, true, this, "OnParameterValueChanged");
	SM->RegisterBoolParameter("OrientRotationToMovementWhenAiming", OrientRotationToMovementWhenAiming, true, this, "OnParameterValueChanged");
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
	if (GetScarletMovement()->GetActiveMovementState() == this)
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
	GetScarletMovement()->GetCharacterMovementComponent()->GetCharacterOwner()->bUseControllerRotationYaw = !GetCharacterMovement()->bOrientRotationToMovement;
}



// RUNNING

// Registering parameters
void USCM_Running::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	SM->RegisterFloatParameter("RunningSpeed", RunningSpeed, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("RunningMaxSpeed", RunningMaxSpeed, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("RunningSpeedBuildUp", RunningSpeedBuildUp, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("RunningSpeedBuildUpWindow", RunningSpeedBuildUpWindow, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("RunningSpeedDrop", RunningSpeedDrop, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("MovementInputIterpolationSpeed_Running", MovementInputIterpolationSpeed, true, this, "OnParameterValueChanged");

	SM->RegisterBoolParameter("OrientRotationToMovement_Running", OrientRotationToMovement, true, this, "OnParameterValueChanged");
	SM->RegisterBoolParameter("OrientRotationToMovementWhenAiming_Running", OrientRotationToMovementWhenAiming, true, this, "OnParameterValueChanged");
}

// Called when any parameter value is changed
void USCM_Running::OnParameterValueChanged(const FName& ParameterName)
{
	if (ParameterName == "RunningSpeed")
		RunningSpeed = GetScarletMovement()->GetFloatParameterValue("RunningSpeed");

	else if (ParameterName == "RunningMaxSpeed")
		RunningMaxSpeed = GetScarletMovement()->GetFloatParameterValue("RunningMaxSpeed");

	else if (ParameterName == "RunningSpeedBuildUp")
		RunningSpeedBuildUp = GetScarletMovement()->GetFloatParameterValue("RunningSpeedBuildUp");

	else if (ParameterName == "RunningSpeedBuildUpWindow")
		RunningSpeedBuildUpWindow = GetScarletMovement()->GetFloatParameterValue("RunningSpeedBuildUpWindow");

	else if (ParameterName == "RunningSpeedDrop")
		RunningSpeedDrop = GetScarletMovement()->GetFloatParameterValue("RunningSpeedDrop");

	else if (ParameterName == "MovementInputIterpolationSpeed_Running")
		MovementInputIterpolationSpeed = GetScarletMovement()->GetFloatParameterValue("MovementInputIterpolationSpeed_Running");

	else if (ParameterName == "OrientRotationToMovement_Running")
		OrientRotationToMovement = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovement_Running");

	else if (ParameterName == "OrientRotationToMovementWhenAiming_Running")
		OrientRotationToMovementWhenAiming = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovementWhenAiming_Running");

	// If the state is active
	if (GetScarletMovement()->GetActiveMovementState() == this)
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
	// Input
	FVector MovementVector = GetScarletMovement()->GetMovementInputVector();
	GetCharacterMovement()->AddInputVector(MovementVector);

	// Speed control
	UCharacterMovementComponent* CM = GetCharacterMovement();
	ACharacter* Character = CM->GetCharacterOwner();

	if (Character->GetVelocity().Size() >= CM->MaxWalkSpeed * RunningSpeedBuildUpWindow)
	{
		if (Character->GetVelocity().Size() < RunningMaxSpeed)
			CM->MaxWalkSpeed = CM->MaxWalkSpeed + RunningSpeedBuildUp * DeltaTime;
		else
			CM->MaxWalkSpeed = RunningMaxSpeed;
	}

	else if (Character->GetVelocity().Size() > RunningSpeed)
		CM->MaxWalkSpeed = CM->MaxWalkSpeed - RunningSpeedDrop * DeltaTime;

	else
		CM->MaxWalkSpeed = RunningSpeed;

	// Orient Rotation To Movement
	bool IsAiming = GetScarletMovement()->GetBoolInputValue("Aim");
	GetCharacterMovement()->bOrientRotationToMovement = IsAiming ? OrientRotationToMovementWhenAiming : OrientRotationToMovement;
	GetScarletMovement()->GetCharacterMovementComponent()->GetCharacterOwner()->bUseControllerRotationYaw = !GetCharacterMovement()->bOrientRotationToMovement;
}


// CROUCHING

// Registering parameters
void USCM_Crouching::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	SM->RegisterFloatParameter("CapsuleHeightMultiplier_Crouching", CapsuleHeightMultiplier, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("CrouchWalkingSpeed", CrouchWalkingSpeed, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("MovementInputIterpolationSpeed_Crouching", MovementInputIterpolationSpeed, true, this, "OnParameterValueChanged");

	SM->RegisterBoolParameter("OrientRotationToMovement_Crouching", OrientRotationToMovement, true, this, "OnParameterValueChanged");
	SM->RegisterBoolParameter("OrientRotationToMovementWhenAiming_Crouching", OrientRotationToMovementWhenAiming, true, this, "OnParameterValueChanged");
}

// Called when any parameter value is changed
void USCM_Crouching::OnParameterValueChanged(const FName& ParameterName)
{
	if (ParameterName == "CapsuleHeightMultiplier_Crouching")
		CapsuleHeightMultiplier = GetScarletMovement()->GetFloatParameterValue("CapsuleHeightMultiplier_Crouching");

	else if (ParameterName == "CrouchWalkingSpeed")
		CrouchWalkingSpeed = GetScarletMovement()->GetFloatParameterValue("CrouchWalkingSpeed");

	else if (ParameterName == "MovementInputIterpolationSpeed_Crouching")
		MovementInputIterpolationSpeed = GetScarletMovement()->GetFloatParameterValue("MovementInputIterpolationSpeed_Crouching");

	else if (ParameterName == "OrientRotationToMovement_Crouching")
		OrientRotationToMovement = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovement_Crouching");

	else if (ParameterName == "OrientRotationToMovementWhenAiming_Crouching")
		OrientRotationToMovementWhenAiming = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovementWhenAiming_Crouching");

	// If the state is active
	if (GetScarletMovement()->GetActiveMovementState() == this)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchWalkingSpeed;
		GetScarletMovement()->MovementInputInterpolationSpeed = MovementInputIterpolationSpeed;
	}
}

void USCM_Crouching::EnterState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	CM->SetMovementMode(EMovementMode::MOVE_Walking);
	CM->MaxWalkSpeed = CrouchWalkingSpeed;
	CM->CrouchedHalfHeight = GetCharacterMovement()->GetCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * CapsuleHeightMultiplier;
	CM->bCanWalkOffLedgesWhenCrouching = false;

	// Actual crouching
	CM->bWantsToCrouch = true;

	GetScarletMovement()->MovementInputInterpolationSpeed = MovementInputIterpolationSpeed;
}

void USCM_Crouching::ExitState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	CM->bWantsToCrouch = false;
}

void USCM_Crouching::UpdateState_Implementation(float DeltaTime)
{
	FVector MovementVector = GetScarletMovement()->GetMovementInputVector();
	GetCharacterMovement()->AddInputVector(MovementVector);

	// Orient Rotation To Movement
	bool IsAiming = GetScarletMovement()->GetBoolInputValue("Aim");

	GetCharacterMovement()->bOrientRotationToMovement = IsAiming ? OrientRotationToMovementWhenAiming : OrientRotationToMovement;
	GetScarletMovement()->GetCharacterMovementComponent()->GetCharacterOwner()->bUseControllerRotationYaw = !GetCharacterMovement()->bOrientRotationToMovement;
}


// SLIDING

// Registering parameters
void USCM_Sliding::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	SM->RegisterFloatParameter("CapsuleHeightMultiplier_Sliding", CapsuleHeightMultiplier, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("SlideBoost", SlideBoost, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("GroundFriction_Sliding", GroundFriction, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("SlideCooldown", SlideCooldown, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("SlideJumpCooldown", SlideJumpCooldown, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("GroundTraceMultiplier_Sliding", GroundTraceMultiplier, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("SlideVelocityConservation", SlideVelocityConservation, true, this, "OnParameterValueChanged");

	SM->RegisterBoolParameter("OrientRotationToMovement_Sliding", OrientRotationToMovement, true, this, "OnParameterValueChanged");
	SM->RegisterBoolParameter("OrientRotationToMovementWhenAiming_Sliding", OrientRotationToMovementWhenAiming, true, this, "OnParameterValueChanged");

	// Sliding cool down
	SM->GetTimerController()->CreateTimer("SlideCooldown", SlideCooldown, false, false);
	SM->GetTimerController()->SubscribeToTimer("SlideCooldown", this, "OnTimerIsOver");

	SM->RegisterDynamicGate("CanSlide", ESCM_DynamicGateRule::And, true);
	SM->SetDynamicGateNamedValue("CanSlide", "SlideCooldown", true);

	// Jump cool down
	SM->GetTimerController()->CreateTimer("SlideJumpCooldown", SlideJumpCooldown, false, false);
	SM->GetTimerController()->SubscribeToTimer("SlideJumpCooldown", this, "OnTimerIsOver");

	if (SM->IsDynamicGateValid("CanJump"))
		SM->SetDynamicGateNamedValue("CanJump", "SlideJumpCooldown", true);
}

bool USCM_Sliding::GroundTrace(FHitResult& OutHit)
{
	auto* Capsule = GetCharacterMovement()->GetCharacterOwner()->GetCapsuleComponent();

	FVector Start = GetScarletMovement()->GetOwner()->GetActorLocation();
	FVector End = Start - FVector::UpVector * Capsule->GetScaledCapsuleHalfHeight() * 1.1f;

	return GetScarletMovement()->GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility);
}

// Called when any parameter value is changed
void USCM_Sliding::OnParameterValueChanged(const FName& ParameterName)
{
	if (ParameterName == "CapsuleHeightMultiplier_Sliding")
		CapsuleHeightMultiplier = GetScarletMovement()->GetFloatParameterValue("CapsuleHeightMultiplier_Sliding");

	else if (ParameterName == "SlideBoost")
		SlideBoost = GetScarletMovement()->GetFloatParameterValue("SlideBoost");

	else if (ParameterName == "GroundFriction_Sliding")
		GroundFriction = GetScarletMovement()->GetFloatParameterValue("GroundFriction_Sliding");

	else if (ParameterName == "SlideVelocityConservation")
		SlideVelocityConservation = GetScarletMovement()->GetFloatParameterValue("SlideVelocityConservation");

	else if (ParameterName == "SlideCooldown")
	{
		SlideCooldown = GetScarletMovement()->GetFloatParameterValue("SlideCooldown");
		GetScarletMovement()->GetTimerController()->ChangeTimerLength("SlideCooldown", SlideCooldown);
	}

	else if (ParameterName == "SlideJumpCooldown")
	{
		SlideJumpCooldown = GetScarletMovement()->GetFloatParameterValue("SlideJumpCooldown");
		GetScarletMovement()->GetTimerController()->ChangeTimerLength("SlideJumpCooldown", SlideJumpCooldown);
	}

	else if (ParameterName == "GroundTraceMultiplier_Sliding")
	{
		float OldGroundTraceMultiplier = GroundTraceMultiplier;
		GroundTraceMultiplier = GetScarletMovement()->GetFloatParameterValue("GroundTraceMultiplier_Sliding");
		if (GetStateMachine()->GetActiveState() == GetStateID())
		{
			GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") / OldGroundTraceMultiplier);
			GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") * GroundTraceMultiplier);
		}
	}

	else if (ParameterName == "OrientRotationToMovement_Sliding")
		OrientRotationToMovement = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovement_Sliding");

	else if (ParameterName == "OrientRotationToMovementWhenAiming_Sliding")
		OrientRotationToMovementWhenAiming = GetScarletMovement()->GetBoolParameterValue("OrientRotationToMovementWhenAiming_Sliding");
}

void USCM_Sliding::OnTimerIsOver(FName InTimerName)
{
	if (InTimerName == "SlideCooldown")
		GetScarletMovement()->SetDynamicGateNamedValue("CanSlide", "SlideCooldown", true);

	else if (InTimerName == "SlideJumpCooldown")
		if (GetScarletMovement()->IsDynamicGateValid("CanJump"))
			GetScarletMovement()->SetDynamicGateNamedValue("CanJump", "SlideJumpCooldown", true);
}

void USCM_Sliding::EnterState_Implementation()
{
	// Movement parameters
	UCharacterMovementComponent* CM = GetCharacterMovement();
	ACharacter* Character = CM->GetCharacterOwner();

	CM->SetMovementMode(EMovementMode::MOVE_Walking);

	auto* Capsule = GetCharacterMovement()->GetCharacterOwner()->GetCapsuleComponent();
	//Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() * CapsuleHeightMultiplier);
	CM->CrouchedHalfHeight = GetCharacterMovement()->GetCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * CapsuleHeightMultiplier;
	CM->bCanWalkOffLedgesWhenCrouching = true;
	CM->bWantsToCrouch = true;


	PreviousGroundFriction = CM->GroundFriction;
	CM->GroundFriction = GroundFriction;

	// Normal sampling
	FVector Normal = FVector::ZeroVector;
	FHitResult Hit;
	if (GroundTrace(Hit))
	{
		Normal = Hit.Normal;
	
		// Velocity
		CM->Velocity = CM->Velocity * SlideVelocityConservation;

		// Slide boost
		FVector Boost = SlideBoost * (-1.f) * FVector::CrossProduct(Normal, GetScarletMovement()->GetOwner()->GetActorRightVector());
		GetCharacterMovement()->GetCharacterOwner()->LaunchCharacter(Boost, false, false);
	}

	// Jump cool down
	if (GetScarletMovement()->IsDynamicGateValid("CanJump"))
	{
		GetScarletMovement()->SetDynamicGateNamedValue("CanJump", "SlideJumpCooldown", false);
		GetScarletMovement()->GetTimerController()->ResetTimer("SlideJumpCooldown");
		GetScarletMovement()->GetTimerController()->StartTimer("SlideJumpCooldown");
	}

	// Ground trace distance
	GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") * GroundTraceMultiplier);
}

void USCM_Sliding::ExitState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	CM->GroundFriction = PreviousGroundFriction;

	CM->bWantsToCrouch = false;
	//auto* Capsule = GetCharacterMovement()->GetCharacterOwner()->GetCapsuleComponent();
	//Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() / CapsuleHeightMultiplier);

	// Slide Cooldown
	GetScarletMovement()->SetDynamicGateNamedValue("CanSlide", "SlideCooldown", false);
	GetScarletMovement()->GetTimerController()->ResetTimer("SlideCooldown");
	GetScarletMovement()->GetTimerController()->StartTimer("SlideCooldown");

	// Ground trace distance
	GetScarletMovement()->SetFloatParameterValue("GroundTraceDistance", GetScarletMovement()->GetFloatParameterValue("GroundTraceDistance") / GroundTraceMultiplier);
}

void USCM_Sliding::UpdateState_Implementation(float DeltaTime)
{
	auto* Character = GetScarletMovement()->GetCharacterMovementComponent()->GetCharacterOwner();

	// Orient Rotation To Movement
	bool IsAiming = GetScarletMovement()->GetBoolInputValue("Aim");

	GetCharacterMovement()->bOrientRotationToMovement = IsAiming ? OrientRotationToMovementWhenAiming : OrientRotationToMovement;
	Character->bUseControllerRotationYaw = !GetCharacterMovement()->bOrientRotationToMovement;

	// Ground suction force
	/*FHitResult Hit;
	if (GroundTrace(Hit))
		GetCharacterMovement()->AddForce(-1.f * Hit.Normal * 10000.f);*/
}