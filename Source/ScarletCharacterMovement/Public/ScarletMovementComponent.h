// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScarletMovementComponent.generated.h"


// Fires off when custom MPAS_Handler parameter value is changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementParameterValueChanged, FName, InParameterName);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCARLETCHARACTERMOVEMENT_API UScarletMovementComponent : public UActorComponent
{
	GENERATED_BODY()


// BASIC

protected:

	// Stack of state machines that is used for controlling movement
	UPROPERTY()
	TArray<class USCM_MovementStateMachine*> MovementStateMachineStack;

	// Character's movement component that needs to be controlled
	class UCharacterMovementComponent* CharacterMovementComponent = nullptr;


public:	
	// Sets default values for this component's properties
	UScarletMovementComponent();

	// Pointer to the active state, should only be modified by movement state machines
	USCM_MovementStateBase* ActiveState;

	// Layers of the movement state machine stack, 0 - lowest layer
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default")
	TArray<TSubclassOf<class USCM_MovementStateMachine>> MovementStateMachineStackClasses;

	// If set to true, attempts to automatically locate an existing character movement component
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool AutoDetectCharacterMovement = true;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Detects an existing character movement component
	class UCharacterMovementComponent* DetectCharacterMovementComponent();

	// Creates and initializes a movement state machine stack
	void InitMovementStateMachineStack();


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Manually sets character movement component
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement")
	void SetCharacterMovementComponent(class UCharacterMovementComponent* InComponent);

	// Returns a pointer to the character movement component that is controlled by this component
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement")
	class UCharacterMovementComponent* GetCharacterMovementComponent() { return CharacterMovementComponent; }


	// Replaces movement state machine stack
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement")
	void RegenerateMovementStateMachineStack(TArray<TSubclassOf<class USCM_MovementStateMachine>> InMovementStateMachineStackClasses);

	// Returns a movement state machine stack that controls the movement component
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement")
	TArray<class USCM_MovementStateMachine*> GetMovementStateMachineStack() { return MovementStateMachineStack; }

	// Returns currently active movement state in the movement stack
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement")
	USCM_MovementStateBase* GetActiveMovementState() { return ActiveState; }


// INPUT

protected:

	// Movement Input Direction, comes from the controller, is processed by the Intention Driver
	FVector MovementInputVector;

	// Used as target value if movement input interpolation is enabled
	FVector TargetMovementInputVector;

	// Determines the desired rotation of the core, comes fromt the controller, is processed by the Intention Driver
	FRotator CameraRotation;

	// Used as target value if camera rotation input interpolation is enabled
	FRotator TargetCameraRotation;

public:

	// Standart movement input vector processing:
	//    - Normalizes input vector, thus fixing diagonal ovelapping
	//    - Keeps analog input range
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Input")
	bool EnableStandartMovementInputProcessing = true;

	// Sets Movement Vector
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Input")
	void SetMovementInputVector(FVector InMovementInput);

	// Returns Movement Input Vector
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	FVector GetMovementInputVector() { return MovementInputVector; }

	// Sets Camera Rotation Input
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Input")
	void SetCurrentCameraRotation(FRotator InCameraRotation);

	// Returns Camera Rotation Input
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	FRotator GetCameraRotation() { return CameraRotation; }


// Interpolation & Processing
protected:

	// Calculates current movement and camera roation input values by interpolating them over time
	void UpdateInputInterpolation(float DeltaTime);

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Input")
	bool OrientMovementInputToYawCameraRotation = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Input")
	bool EnableMovementInputInterpolation = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Input")
	float MovementInputInterpolationSpeed = 5.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Input")
	bool EnableCameraRotationInputInterpolation = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Input")
	float CameraRotationInputInterpolationSpeed = 5.f;


// Custom Inputs

protected:

	UPROPERTY()
	TMap<FName, bool> BoolInputStorage = { {"Default", false} };

	UPROPERTY()
	TMap<FName, int32> IntInputStorage = { {"Default", 0} };

	UPROPERTY()
	TMap<FName, float> FloatInputStorage = { {"Default", 0.f} };

	UPROPERTY()
	TMap<FName, FVector> VectorInputStorage = { {"Default", FVector::ZeroVector} };

	UPROPERTY()
	TMap<FName, FRotator> RotatorInputStorage = { {"Default", FRotator::ZeroRotator} };

protected:

	// Sets <T> Input in given Map:
	template< typename T >
	void SetInputValue(TMap<FName, T>& InputStorage, const FName& InputName, const T& Value)
	{
		InputStorage.Add(InputName, Value);
	}

	// Gets <T> Input from given Map
	template< typename T >
	const T& GetInputValue(TMap<FName, T>& InputStorage, const FName& InputName)
	{
		T* ValueP = InputStorage.Find(InputName);
		if (ValueP)
			return *ValueP;

		return InputStorage["Default"];
	}

public:

	// Sets a bool-type input value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Input")
	void SetBoolInputValue(const FName& InInputName, bool InValue) { SetInputValue<bool>(BoolInputStorage, InInputName, InValue); }

	// Returns a bool-type input value (returns default value if no such input is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	bool GetBoolInputValue(const FName& InInputName) { return GetInputValue<bool>(BoolInputStorage, InInputName); }


	// Sets a Int-type input value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Input")
	void SetIntInputValue(const FName& InInputName, int32 InValue) { SetInputValue<int32>(IntInputStorage, InInputName, InValue); }

	// Returns a Int-type input value (returns default value if no such input is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	int32 GetIntInputValue(const FName& InInputName) { return GetInputValue<int32>(IntInputStorage, InInputName); }


	// Sets a Float-type input value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Input")
	void SetFloatInputValue(const FName& InInputName, float InValue) { SetInputValue<float>(FloatInputStorage, InInputName, InValue); }

	// Returns a Float-type input value (returns default value if no such input is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	float GetFloatInputValue(const FName& InInputName) { return GetInputValue<float>(FloatInputStorage, InInputName); }


	// Sets a Vector-type input value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Input")
	void SetVectorInputValue(const FName& InInputName, const FVector& InValue) { SetInputValue<FVector>(VectorInputStorage, InInputName, InValue); }

	// Returns a Vector-type input value (returns default value if no such input is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	FVector GetVectorInputValue(const FName& InInputName) { return GetInputValue<FVector>(VectorInputStorage, InInputName); }


	// Sets a Rotator-type input value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Input")
	void SetRotatorInputValue(const FName& InInputName, const FRotator& InValue) { SetInputValue<FRotator>(RotatorInputStorage, InInputName, InValue); }

	// Returns a Rotator-type input value (returns default value if no such input is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	FRotator GetRotatorInputValue(const FName& InInputName) { return GetInputValue<FRotator>(RotatorInputStorage, InInputName); }



// PARAMETERS

protected:

	UPROPERTY()
	TMap<FName, bool> BoolParameterStorage = { {"Default", false} };

	UPROPERTY()
	TMap<FName, int32> IntParameterStorage = { {"Default", 0} };

	UPROPERTY()
	TMap<FName, float> FloatParameterStorage = { {"Default", 0.f} };


	// List of parameter subscriptions
	UPROPERTY()
	TMap<FName, FOnMovementParameterValueChanged> ParameterSubscriptions;

protected:

	// Sends notifications to parameter's subscribers
	void SendParameterNotifications(const FName& InParamterName);

	// Sets <T> Parameter in given Map:
	template< typename T >
	void SetParameterValue(TMap<FName, T>& ParameterStorage, const FName& ParameterName, const T& Value)
	{
		ParameterStorage.Add(ParameterName, Value);
		SendParameterNotifications(ParameterName);
	}

	// Gets <T> Parameter from given Map
	template< typename T >
	const T& GetParameterValue(TMap<FName, T>& ParameterStorage, const FName& ParameterName)
	{
		T* ValueP = ParameterStorage.Find(ParameterName);
		if (ValueP)
			return *ValueP;

		return ParameterStorage["Default"];
	}

public:

	// Subscribes the given rig element to the given custom parameter's update notifications
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void SubscribeToParameter(FName ParameterName, UObject* Subscriber, FName NotificationFunctionName);

	// Returns name lists for all movement parameters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	void GetAllParameterNames(TArray<FName>& BoolParameters, TArray<FName>& IntParameters, TArray<FName>& FloatParameters);

	// Sets a bool-type input value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void SetBoolParameterValue(const FName& InParameterName, bool InValue) { SetParameterValue<bool>(BoolParameterStorage, InParameterName, InValue); }

	// Returns a bool-type input value (returns default value if no such input is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	bool GetBoolParameterValue(const FName& InParameterName) { return GetParameterValue<bool>(BoolParameterStorage, InParameterName); }

	// Whether the specified bool-type parameter exists or not
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	bool IsBoolParameterValid(const FName& InParameterName) { return BoolParameterStorage.Contains(InParameterName); }


	// Sets a Int-type input value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void SetIntParameterValue(const FName& InParameterName, int32 InValue) { SetParameterValue<int32>(IntParameterStorage, InParameterName, InValue); }

	// Returns a Int-type input value (returns default value if no such input is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	int32 GetIntParameterValue(const FName& InParameterName) { return GetParameterValue<int32>(IntParameterStorage, InParameterName); }

	// Whether the specified Int-type parameter exists or not
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	bool IsIntParameterValid(const FName& InParameterName) { return IntParameterStorage.Contains(InParameterName); }


	// Sets a Float-type input value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void SetFloatParameterValue(const FName& InParameterName, float InValue) { SetParameterValue<float>(FloatParameterStorage, InParameterName, InValue); }

	// Returns a Float-type input value (returns default value if no such input is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	float GetFloatParameterValue(const FName& InParameterName) { return GetParameterValue<float>(FloatParameterStorage, InParameterName); }

	// Whether the specified Float-type parameter exists or not
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	bool IsFloatParameterValid(const FName& InParameterName) { return FloatParameterStorage.Contains(InParameterName); }
};
