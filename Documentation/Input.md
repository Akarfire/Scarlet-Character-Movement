
### Default Input

#### Default Input Values

Default input values include:
* **Movement Input Vector** - direction, in which the character is supposed to be moving;
* **Camera Rotation** - current rotation of the camera (does not affect the camera rotation, only used for calculating movement logic);

---
#### Default Input Processing

There are several input processing techniques that can be applied to the default input values:

###### 1. Standard Movement Input Processing:
Normalizes movement input vector, while maintaining analog value range:
```c++
FVector InputDirection = InputVector.GetSafeNormal();
FVector OutVector = InputVector * InputDirection.GetAbs();

return OutVector;
```

Can be enabled/disabled by modifying the following field:
```c++
bool EnableStandardMovementInputProcessing = true;
```


###### 2. Orient Movement Input To Yaw Camera Rotation
Orients movement input vector to match player camera's perspective:
```c++
MovementInput = MovementInput.RotateAngleAxis(GetCameraRotation().Yaw, FVector::UpVector);
```

Can be enabled/disabled by modifying the following field:
```c++
bool OrientMovementInputToYawCameraRotation = true;
```


###### 3. Input Interpolation
Interpolates input values over time to smooth out rapid changes:
```c++
MovementInputVector = UKismetMathLibrary::VInterpTo(MovementInputVector, TargetMovementInputVector, DeltaTime, MovementInputInterpolationSpeed);

CameraRotation = UKismetMathLibrary::RInterpTo(CameraRotation, TargetCameraRotation, DeltaTime, CameraRotationInputInterpolationSpeed);
```

Can be controlled by the following parameters:
```c++
// Movement Input Settings
bool EnableMovementInputInterpolation = false;
float MovementInputInterpolationSpeed = 5.f;

// Camera Input settings
bool EnableCameraRotationInputInterpolation = false;
float CameraRotationInputInterpolationSpeed = 5.f;
```

---
#### Default Input Interface

Default input values can be accessed with the following interface:

```c++
// Sets Movement Vector
void SetMovementInputVector(FVector InMovementInput);

// Returns Movement Input Vector
FVector GetMovementInputVector();

// Returns Uninterpolated Movement Input Vector
FVector GetRawMovementInputVector();
```

```c++
// Sets Camera Rotation Input
void SetCurrentCameraRotation(FRotator InCameraRotation);

// Returns Camera Rotation Input
FRotator GetCameraRotation();

// Returns Uninterpolated Camera Rotation Vector
FRotator GetRawCameraRotation();
```

---


### Custom Input

In order to handle custom input values (any input value different from the *Default Input* values) the system uses Named input values of the following types:
`bool`, `int32`, `float`, `FVector`, `FRotator`.

#### Interface

To access/modify *Custom Input* values the following interface can be used:

```c++
// Sets a bool-type input value
void SetBoolInputValue(const FName& InInputName, bool InValue);

// Returns a bool-type input value (returns default value if no such input is found)
bool GetBoolInputValue(const FName& InInputName);
```

```c++
// Sets a Int-type input value
void SetIntInputValue(const FName& InInputName, int32 InValue);

// Returns a Int-type input value (returns default value if no such input is found)
int32 GetIntInputValue(const FName& InInputName);
```

```c++
// Sets a Float-type input value
void SetFloatInputValue(const FName& InInputName, float InValue);

// Returns a Float-type input value (returns default value if no such input is found)
float GetFloatInputValue(const FName& InInputName);
```

```c++
// Sets a Vector-type input value
void SetVectorInputValue(const FName& InInputName, const FVector& InValue);

// Returns a Vector-type input value (returns default value if no such input is found)
FVector GetVectorInputValue(const FName& InInputName);
```

```c++
// Sets a Rotator-type input value
void SetRotatorInputValue(const FName& InInputName, const FRotator& InValue);

// Returns a Rotator-type input value (returns default value if no such input is found)
FRotator GetRotatorInputValue(const FName& InInputName);
```

---
#### Internals

Internally *Custom Input* values are stored in `TMap` type storages:

```c++
TMap<FName, bool> BoolInputStorage = { {"Default", false} };

TMap<FName, int32> IntInputStorage = { {"Default", 0} };

TMap<FName, float> FloatInputStorage = { {"Default", 0.f} };

TMap<FName, FVector> VectorInputStorage = { {"Default", FVector::ZeroVector} };

TMap<FName, FRotator> RotatorInputStorage = { {"Default", FRotator::ZeroRotator} };
```

Each storage has a `"Default"` value that is used when accessing an input value that does not exist.

Templated methods are used to access/modify values inside the storages:
```c++
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
```

*Example implementation for bool type input values:*
```c++
// Sets a bool-type input value
void SetBoolInputValue(const FName& InInputName, bool InValue) 
{ 
	SetInputValue<bool>(BoolInputStorage, InInputName, InValue); 
}

// Returns a bool-type input value (returns default value if no such input is found)
bool GetBoolInputValue(const FName& InInputName) 
{ 
	return GetInputValue<bool>(BoolInputStorage, InInputName); 
}
```

---
#### Debugging

To make debugging input values easier the following methods are provided:

```c++
void GetAllBoolInputNames(TArray<FName>& OutNames);

void GetAllIntInputNames(TArray<FName>& OutNames);

void GetAllFloatInputNames(TArray<FName>& OutNames);

void GetAllVectorInputNames(TArray<FName>& OutNames);

void GetAllRotatorInputNames(TArray<FName>& OutNames);
```

The methods above return arrays of names of all parameters of the specified type.

