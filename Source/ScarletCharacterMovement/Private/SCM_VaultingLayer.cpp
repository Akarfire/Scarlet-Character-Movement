// Fill out your copyright notice in the Description page of Project Settings.


#include "SCM_VaultingLayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"

// VAULTING

// Registering parameters
void USCM_Vaulting::SetupParameters_Implementation()
{
	UScarletMovementComponent* SM = GetScarletMovement();

	// Parameters
	SM->RegisterFloatParameter("VaultingSnapAnimationPlaySpeed", VaultingSnapAnimationPlaySpeed, true, this, "OnParameterValueChanged");
	SM->RegisterFloatParameter("VaultingDuration", VaultingDuration, true, this, "OnParameterValueChanged");
	// ...
	
	// Timers & Timelines
	USTT_TimerController* TC = SM->GetTimerController();
	TC->CreateTimeline("VaultSnap", 1.f, false, false);
	TC->SetTimelinePlaybackSpeed("VaultSnap", VaultingSnapAnimationPlaySpeed);
	TC->SubscribeToTimeline("VaultSnap", this, "OnSnapAnimationUpdated", "OnSnapAnimationFinished");

	TC->CreateTimer("VaultDuration", VaultingDuration, false, false);
	TC->SubscribeToTimer("VaultDuration", this, "OnTimerIsOver");
	// ...

	// Dynamic gates
	if (SM->IsDynamicGateValid("StopVault"))
		SM->SetDynamicGateNamedValue("StopVault", "VaultDuration", false);
	// ...
}

// Called when any parameter value is changed
void USCM_Vaulting::OnParameterValueChanged(const FName& ParameterName)
{
	UScarletMovementComponent* SM = GetScarletMovement();
	if (ParameterName == "VaultingSnapAnimationPlaySpeed")
	{
		VaultingSnapAnimationPlaySpeed = SM->GetFloatParameterValue("VaultingSnapAnimationPlaySpeed");
		SM->GetTimerController()->SetTimelinePlaybackSpeed("VaultSnap", VaultingSnapAnimationPlaySpeed);
	}

	else if (ParameterName == "VaultDuration")
	{
		VaultingDuration = SM->GetFloatParameterValue("VaultDuration");
		SM->GetTimerController()->ChangeTimerLength("VaultDuration", VaultingDuration);
	}
}

void USCM_Vaulting::OnTimerIsOver(FName TimerName)
{
	if (TimerName == "VaultDuration")
	{
		if (GetScarletMovement()->IsDynamicGateValid("StopVault"))
			GetScarletMovement()->SetDynamicGateNamedValue("StopVault", "VaultDuration", true);
	}
}

void USCM_Vaulting::OnSnapAnimationUpdated(FName TimelineName, float CurrentTime)
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	ACharacter* Character = CM->GetCharacterOwner();

	Character->SetActorLocation(UKismetMathLibrary::VLerp(SnapInitialLocation, SnapTargetLocation, CurrentTime));

	FRotator InitialRotation = Character->GetActorRotation();
	InitialRotation.Yaw = SnapInitialZRotation;

	FRotator TargetRotation = Character->GetActorRotation();
	TargetRotation.Yaw = SnapTargetZRotation;

	Character->SetActorRotation(UKismetMathLibrary::RLerp(InitialRotation, TargetRotation, CurrentTime, true));
}

void USCM_Vaulting::OnSnapAnimationFinished(FName TimelineName)
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	CM->SetMovementMode(EMovementMode::MOVE_Flying);

	GetScarletMovement()->GetTimerController()->StartTimer("VaultDuration");
}

void USCM_Vaulting::EnterState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	ACharacter* Character = CM->GetCharacterOwner();

	CM->GravityScale = 0.0f;

	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Character->StopJumping();
	Character->LaunchCharacter(FVector::ZeroVector, true, true);

	if (GetScarletMovement()->IsDynamicGateValid("StopVault"))
		GetScarletMovement()->SetDynamicGateNamedValue("StopVault", "VaultDuration", false);

	// Animation values
	SnapInitialLocation = Character->GetActorLocation();
	SnapInitialZRotation = Character->GetActorRotation().Yaw;

	// Start Animation
	GetScarletMovement()->GetTimerController()->ResetTimeline("VaultSnap");
	GetScarletMovement()->GetTimerController()->StartTimeline("VaultSnap");
}

void USCM_Vaulting::ExitState_Implementation()
{
	UCharacterMovementComponent* CM = GetCharacterMovement();
	ACharacter* Character = CM->GetCharacterOwner();

	CM->GravityScale = 1.0f;
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CM->SetMovementMode(EMovementMode::MOVE_Walking);
}

void USCM_Vaulting::UpdateState_Implementation(float DeltaTime)
{
	// Animation values
	SnapTargetLocation = VaultLocation;
	SnapTargetZRotation = (WallNormal * -1.f).Rotation().Yaw;
}


// STATE MACHINE

bool USCM_VaultingLayer::CheckCanVault()
{
	ACharacter* Character = GetCharacterMovement()->GetCharacterOwner();

	FVector Start = Character->GetActorLocation();
	FVector End = Start + Character->GetActorForwardVector() * WallTraceDistance;

	// Detecting a wall
	FHitResult WallHit;
	FCollisionObjectQueryParams CollisionParams;
	CollisionParams.AddObjectTypesToQuery(ECC_WorldStatic);
	if (GetWorld()->LineTraceSingleByObjectType(WallHit, Start, End, CollisionParams))
	{
		//DrawDebugSphere(GetWorld(), WallHit.Location, 100.f, 0, FColor::Red, false, 0.1f, (uint8)0U, 10.f);

		// Caching wall normal
		WallNormal = WallHit.Normal;

		// Calcuate Height trace end (extended position)
		FVector RelativeVector = (WallHit.Location - Start);
		FVector HeightTraceEnd = Start + (RelativeVector / (RelativeVector.Size() + 0.0001f)) * (RelativeVector.Size() + InWallExtention);
		FVector HeightTraceStart = HeightTraceEnd + FVector(0.f, 0.f, MaxWallHeight);

		// Detecting ground on top of the wall
		FHitResult HeightHit;
		if (GetWorld()->LineTraceSingleByObjectType(HeightHit, HeightTraceStart, HeightTraceEnd, CollisionParams))
		{
			//UCapsuleComponent* DebugCapsule = Character->GetCapsuleComponent();
			//DrawDebugCapsule(GetWorld(), HeightHit.Location + FVector(0.f, 0.f, DebugCapsule->GetScaledCapsuleHalfHeight()), DebugCapsule->GetScaledCapsuleHalfHeight(), DebugCapsule->GetUnscaledCapsuleRadius(), FQuat(FRotator()), FColor::Blue, false, 0.1f, (uint8)0U, 2.f);

			// Check for ground stiffnes
			if (UKismetMathLibrary::Dot_VectorVector(HeightHit.Normal, FVector(0.f, 0.f, 1.f)) >= MinGroundLevelness)
			{
				UCapsuleComponent* Capsule = Character->GetCapsuleComponent();

				// Check if character can fit
				FVector CheckLocaiton = HeightHit.Location + FVector(0.f, 0.f, Capsule->GetScaledCapsuleHalfHeight() + GroundCheckTollerance);
				VaultLocation = CheckLocaiton - FVector(0.f, 0.f, GroundCheckTollerance) + WallNormal * InWallExtention; // Caching vault location

				if (!GetWorld()->OverlapBlockingTestByChannel(	CheckLocaiton,
																Character->GetActorQuat(),
																Capsule->GetCollisionObjectType(),
																Capsule->GetCollisionShape()))
				{
					return true; // All checks were successful
				}
			}
		}
	}

	return false;
}
