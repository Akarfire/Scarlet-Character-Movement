// Fill out your copyright notice in the Description page of Project Settings.


#include "ScarletMovementComponent.h"
#include "SCM_MovementStateMachine.h"
#include "SCM_StateMachineInterface.h"
#include "SCM_MovementStateBase.h"
#include "ScarletMovementFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UScarletMovementComponent::UScarletMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}


// Called when the game starts
void UScarletMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// Automatically detecting movement component if needed
	if (AutoDetectCharacterMovement)
		SetCharacterMovementComponent(DetectCharacterMovementComponent());
	
	// Initializing movement state machine stack
	InitMovementStateMachineStack();
}

// Detects an existing character movement component
UCharacterMovementComponent* UScarletMovementComponent::DetectCharacterMovementComponent()
{
	UActorComponent* MovementComponent_ActorComponent = GetOwner()->FindComponentByClass(UCharacterMovementComponent::StaticClass());
	if (MovementComponent_ActorComponent)
	{
		UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(MovementComponent_ActorComponent);
		if (MovementComponent)
			return MovementComponent;
	}

	return nullptr;
}

// Creates and initializes a movement state machine stack
void UScarletMovementComponent::InitMovementStateMachineStack()
{
	MovementStateMachineStack.Reset();

	for (int32 i = 0; i < MovementStateMachineStackClasses.Num(); i++)
	{
		auto& MovementStateMachineClass = MovementStateMachineStackClasses[i];
		USCM_MovementStateMachine* MovementStateMachine = NewObject<USCM_MovementStateMachine>(this, MovementStateMachineClass);

		// Initializing state machine
		if (MovementStateMachine)
		{
			MovementStateMachine->MOVEMENTSTATEMACHINE_SetScarletMovement(this);
			MovementStateMachine->InitStateMachine();

			for (auto& State : MovementStateMachine->GetStates())
			{
				USCM_MovementStateBase* MovementState = Cast<USCM_MovementStateBase>(State.Value);
				if (MovementState)
					MovementState->SetupParameters();
			}
		}

		// Linking with the previous layer
		if (i > 0 && MovementStateMachine->GetClass()->ImplementsInterface(USCM_StateMachineInterface::StaticClass()))
		{
			USSM_NestedStateMachineState* LowerLayerContainer = ISCM_StateMachineInterface::Execute_GetLowerLayerContainerState(MovementStateMachine);
			LowerLayerContainer->SetEmbeddedStateMachine(MovementStateMachineStack[i - 1]);
		}

		MovementStateMachineStack.Add(MovementStateMachine);
	}
}


// Called every frame
void UScarletMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive()) return;

	// Input interpolation
	UpdateInputInterpolation(DeltaTime);

	// Updating movement state machine stack
	if (!MovementStateMachineStack.IsEmpty())
		MovementStateMachineStack.Last()->UpdateStateMachine(DeltaTime);
}

// Manually sets character movement component
void UScarletMovementComponent::SetCharacterMovementComponent(UCharacterMovementComponent* InComponent)
{
	CharacterMovementComponent = InComponent;
}

// Replaces movement state machine stack
void UScarletMovementComponent::RegenerateMovementStateMachineStack(TArray<TSubclassOf<class USCM_MovementStateMachine>> InMovementStateMachineStackClasses)
{
	MovementStateMachineStackClasses = InMovementStateMachineStackClasses;
	InitMovementStateMachineStack();
}


// INPUT

// Sets Movement Vector
void UScarletMovementComponent::SetMovementInputVector(FVector InMovementInput)
{
	FVector MovementInput = InMovementInput;
	if (EnableStandartMovementInputProcessing)
		MovementInput = UScarletMovementFunctionLibrary::ProcessMovementInputVector(InMovementInput);

	if (OrientMovementInputToYawCameraRotation)
		MovementInput = MovementInput.RotateAngleAxis(GetCameraRotation().Yaw, FVector::UpVector);

	if (EnableMovementInputInterpolation)
		TargetMovementInputVector = MovementInput;

	else
		MovementInputVector = MovementInput;
}

// Sets Camera Rotation Input
void UScarletMovementComponent::SetCurrentCameraRotation(FRotator InCameraRotation)
{
	if (EnableCameraRotationInputInterpolation)
		TargetCameraRotation = InCameraRotation;

	else
		CameraRotation = InCameraRotation;
}

// Calculates current movement and camera roation input values by interpolating them over time
void UScarletMovementComponent::UpdateInputInterpolation(float DeltaTime)
{
	if (EnableMovementInputInterpolation)
		MovementInputVector = UKismetMathLibrary::VInterpTo(MovementInputVector, TargetMovementInputVector, DeltaTime, MovementInputInterpolationSpeed);

	if (EnableCameraRotationInputInterpolation)
		CameraRotation = UKismetMathLibrary::RInterpTo(CameraRotation, TargetCameraRotation, DeltaTime, CameraRotationInputInterpolationSpeed);
}


// PARAMTERS

void UScarletMovementComponent::SendParameterNotifications(const FName& InParameterName)
{
	FOnMovementParameterValueChanged* DelegateP = ParameterSubscriptions.Find(InParameterName);
	if (DelegateP)
		(*DelegateP).Broadcast(InParameterName);
}

void UScarletMovementComponent::SubscribeToParameter(FName ParameterName, UObject* Subscriber, FName NotificationFunctionName)
{
	if (Subscriber)
	{
		TScriptDelegate Delegate;
		Delegate.BindUFunction(Subscriber, NotificationFunctionName);

		if (!ParameterSubscriptions.Contains(ParameterName))
			ParameterSubscriptions.Add(ParameterName, FOnMovementParameterValueChanged());

		ParameterSubscriptions[ParameterName].Add(Delegate);
	}
}

// Returns name lists for all movement parameters
void UScarletMovementComponent::GetAllParameterNames(TArray<FName>& BoolParameters, TArray<FName>& IntParameters, TArray<FName>& FloatParameters)
{
	BoolParameterStorage.GetKeys(BoolParameters);
	IntParameterStorage.GetKeys(IntParameters);
	FloatParameterStorage.GetKeys(FloatParameters);
}
