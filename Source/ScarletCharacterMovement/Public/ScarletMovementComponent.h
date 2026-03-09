// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STT_TimerController.h"
#include "DynamicGates.h"
#include "ScarletMovementComponent.generated.h"


// Fires off when custom MPAS_Handler parameter value is changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementParameterValueChanged, FName, InParameterName);

// Fires off when Movement State has changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMovementStateChanged, FString, NewMovementStateName, USCM_MovementStateBase*, NewMovementState);


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
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

	// Scarlet_TimersAndTimelines integration
	// Timer controller that handles custom timers and timelines
	USTT_TimerController* TimerController = nullptr;

public:	
	// Sets default values for this component's properties
	UScarletMovementComponent();

	// Pointer to the active state, should only be modified by movement state machines
	USCM_MovementStateBase* ActiveState;

	// DO NOT CALL MANUALLY!
	void SetActiveState(USCM_MovementStateBase* NewActiveState);

	// Layers of the movement state machine stack, 0 - lowest layer
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default")
	TArray<TSubclassOf<class USCM_MovementStateMachine>> MovementStateMachineStackClasses;

	// If set to true, attempts to automatically locate an existing character movement component
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	bool AutoDetectCharacterMovement = true;

	// Fires off when Movement State has changed
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnMovementStateChanged OnMovementStateChanged;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Detects an existing character movement component
	class UCharacterMovementComponent* DetectCharacterMovementComponent();

	// Creates and initializes a movement state machine stack
	void InitMovementStateMachineStack();

	// Scarlet_TimersAndTimelines integration
	// Locates an existing timer controller component or creates a new one
	void LocateOrCreateTimerController();


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

	// Scarlet_TimersAndTimelines integration
	// Timer controller that handles custom timers and timelines
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement")
	USTT_TimerController* GetTimerController() { return TimerController; }

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
	bool EnableStandardMovementInputProcessing = true;

	// Sets Movement Vector
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Input")
	void SetMovementInputVector(FVector InMovementInput);

	// Returns Movement Input Vector
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	FVector GetMovementInputVector() { return MovementInputVector; }

	// Returns Uninterpolated Movement Input Vector
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	FVector GetRawMovementInputVector() { return EnableMovementInputInterpolation ? TargetMovementInputVector : MovementInputVector; }

	// Sets Camera Rotation Input
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Input")
	void SetCurrentCameraRotation(FRotator InCameraRotation);

	// Returns Camera Rotation Input
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	FRotator GetCameraRotation() { return CameraRotation; }

	// Returns Uninterpolated Camera Rotation Vector
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input")
	FRotator GetRawCameraRotation() { return EnableCameraRotationInputInterpolation ? TargetCameraRotation : CameraRotation; }


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


	// DEBUG
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input|Debug")
	void GetAllBoolInputNames(TArray<FName>& OutNames) { BoolInputStorage.GetKeys(OutNames); }

	// DEBUG
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input|Debug")
	void GetAllIntInputNames(TArray<FName>& OutNames) { IntInputStorage.GetKeys(OutNames); }

	// DEBUG
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input|Debug")
	void GetAllFloatInputNames(TArray<FName>& OutNames) { FloatInputStorage.GetKeys(OutNames); }

	// DEBUG
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input|Debug")
	void GetAllVectorInputNames(TArray<FName>& OutNames) { VectorInputStorage.GetKeys(OutNames); }

	// DEBUG
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Input|Debug")
	void GetAllRotatorInputNames(TArray<FName>& OutNames) { RotatorInputStorage.GetKeys(OutNames); }



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

	// Checks if the specified parameter exists and creates it if not
	// Can also register a subsciption to the parameter
	template< typename T>
	void RegisterParameter(	TMap<FName, T>& ParameterStorage, const FName& ParameterName, const T& DefaultValue, 
							bool AutoSubscribe, UObject* Subscriber, FName NotificationFunctionName)
	{
		if (!ParameterStorage.Contains(ParameterName))
			ParameterStorage.Add(ParameterName, DefaultValue);

		if (AutoSubscribe)
			SubscribeToParameter(ParameterName, Subscriber, NotificationFunctionName);
	}

public:

	// Subscribes the given rig element to the given custom parameter's update notifications
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void SubscribeToParameter(FName ParameterName, UObject* Subscriber, FName NotificationFunctionName);

	// Returns name lists for all movement parameters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	void GetAllParameterNames(TArray<FName>& BoolParameters, TArray<FName>& IntParameters, TArray<FName>& FloatParameters);


	// Registers a new parameter, initialized with a default value. Also provides means to subscribe to the created paramter right away
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void RegisterBoolParameter(	const FName& ParameterName, bool DefaultValue,
								bool AutoSubscribe = false, UObject* Subscriber = nullptr, FName NotificationFunctionName = "")
	{
		RegisterParameter<bool>(BoolParameterStorage, ParameterName, DefaultValue, AutoSubscribe, Subscriber, NotificationFunctionName);
	}

	// Sets a bool-type parameter value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void SetBoolParameterValue(const FName& InParameterName, bool InValue) { SetParameterValue<bool>(BoolParameterStorage, InParameterName, InValue); }

	// Returns a bool-type parameter value (returns default value if no such parameter is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	bool GetBoolParameterValue(const FName& InParameterName) { return GetParameterValue<bool>(BoolParameterStorage, InParameterName); }

	// Whether the specified bool-type parameter exists or not
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	bool IsBoolParameterValid(const FName& InParameterName) { return BoolParameterStorage.Contains(InParameterName); }


	// Registers a new parameter, initialized with a default value. Also provides means to subscribe to the created paramter right away
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void RegisterIntParameter(const FName& ParameterName, int32 DefaultValue,
		bool AutoSubscribe = false, UObject* Subscriber = nullptr, FName NotificationFunctionName = "")
	{
		RegisterParameter<int32>(IntParameterStorage, ParameterName, DefaultValue, AutoSubscribe, Subscriber, NotificationFunctionName);
	}

	// Sets a Int-type parameter value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void SetIntParameterValue(const FName& InParameterName, int32 InValue) { SetParameterValue<int32>(IntParameterStorage, InParameterName, InValue); }

	// Returns a Int-type parameter value (returns default value if no such parameter is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	int32 GetIntParameterValue(const FName& InParameterName) { return GetParameterValue<int32>(IntParameterStorage, InParameterName); }

	// Whether the specified Int-type parameter exists or not
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	bool IsIntParameterValid(const FName& InParameterName) { return IntParameterStorage.Contains(InParameterName); }


	// Registers a new parameter, initialized with a default value. Also provides means to subscribe to the created paramter right away
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void RegisterFloatParameter(const FName& ParameterName, float DefaultValue,
		bool AutoSubscribe = false, UObject* Subscriber = nullptr, FName NotificationFunctionName = "")
	{
		RegisterParameter<float>(FloatParameterStorage, ParameterName, DefaultValue, AutoSubscribe, Subscriber, NotificationFunctionName);
	}

	// Sets a Float-type parameter value
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|Parameters")
	void SetFloatParameterValue(const FName& InParameterName, float InValue) { SetParameterValue<float>(FloatParameterStorage, InParameterName, InValue); }

	// Returns a Float-type parameter value (returns default value if no such parameter is found)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	float GetFloatParameterValue(const FName& InParameterName) { return GetParameterValue<float>(FloatParameterStorage, InParameterName); }

	// Whether the specified Float-type parameter exists or not
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|Parameters")
	bool IsFloatParameterValid(const FName& InParameterName) { return FloatParameterStorage.Contains(InParameterName); }



// DYNAMIC GATES
protected:

	// Map of dynamic gates
	TMap<FName, FSCM_DynamicGate> DynamicGates;

public:

	// Registers a new dynamic gate, return true if succeded. If a gate with the specified name already exists return "false"
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|DynamicGates")
	bool RegisterDynamicGate(const FName& GateName, ESCM_DynamicGateRule Rule, bool DefaultValue) 
	{ 
		if (DynamicGates.Contains(GateName)) 
			return false;
		DynamicGates.Add(GateName, FSCM_DynamicGate(Rule, DefaultValue));
		return true;
	}

	// Whether a dynamic gate with the specified name exists or not
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|DynamicGates")
	bool IsDynamicGateValid(const FName& GateName) { return DynamicGates.Contains(GateName); }

	// Returns the resulting value of the dynamic gate, "false" if not found
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|DynamicGates")
	bool GetDynamicGateValue(const FName& GateName)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		if (GateP)
			return GateP->GetResultingGateValue();
		return false;
	}


	// Returns the rule of the specified dynamic gate (Rule that is used to combine gate values to calculate the resulting value)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|DynamicGates")
	ESCM_DynamicGateRule GetDynamicGateRule(const FName& GateName)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		check(GateP);
		return GateP->Rule;
	}

	// Updates gate's rule (Rule that is used to combine gate values to calculate the resulting value)
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|DynamicGates")
	void SetDynamicGateRule(const FName& GateName, ESCM_DynamicGateRule NewRule)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		check(GateP);
		GateP->SetGateRule(NewRule);
	}


	// Adds/Updates a named value in the dynamic gate
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|DynamicGates")
	void SetDynamicGateNamedValue(const FName& GateName, const FName& ValueName, bool Value)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		check(GateP);
		GateP->SetNamedGateValue(ValueName, Value);
	}

	// Removes a named value from the dynamic gate
	UFUNCTION(BlueprintCallable, Category = "ScarletMovement|DynamicGates")
	void RemoveDynamicGateNamedValue(const FName& GateName, const FName& ValueName)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		check(GateP);
		GateP->RemoveNamedGateValue(ValueName);
	}

	// Returns the named value from the dynamic gate
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|DynamicGates")
	bool GetDynamicGateNamedValue(const FName& GateName, const FName& ValueName)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		check(GateP);
		bool* ValueP = GateP->GetNamedGateValue(ValueName);
		check(ValueP);
		return *ValueP;
	}

	// DEBUG
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|DynamicGates|Debug")
	void GetAllDynamicGateNames(TArray<FName>& OutNames) { DynamicGates.GetKeys(OutNames); }

	// DEBUG
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScarletMovement|DynamicGates|Debug")
	void GetDynamicGateValueNames(const FName& GateName, TArray<FName>& OutNames)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		if (GateP)
		{
			GateP->GateValues.GetKeys(OutNames);
		}
	}
};
