### Concept

In order to maintain centralized control of any number of movement layers and states Scarlet Character Movement system uses dynamic "on changed notification"-based parameters of the following types: `bool`, `int32`, `float`.

Whenever parameter's value is changed, a notification is sent to all `UObject`s that are subscribed to it. 

***Subscribers are meant to use a cached copy of the parameter's value and updated only when the notification is received.***

### Usage Pattern

1. Create a variable;
2. Register a parameter and initialize it with the variable value;
3. Subscribe to the registered parameter;
4. Update the variable value by fetching parameter value (`GetTypeParameterValue`) when a ''on-value-changed' notification is received;
5. Use the variable as the parameter's value.

---
### Interface

#### General

Each parameter type can be utilized with the following interface (**type** can be `bool`, `int32` or `float`):

*1.* Registers a new parameter, initialized with a default value. Also provides means to subscribe to the created parameter right away. If the specified parameter already exists, then ONLY auto-subscription logic is executed (no errors occur).
```cpp
void RegisterTypeParameter( const FName& ParameterName, 
							type DefaultValue,
							bool AutoSubscribe = false, 
							UObject* Subscriber = nullptr, 
							FName NotificationFunctionName = "");
```

*2.* Updates a parameter value. If the specified parameter does not exist, then it is registered and initialized with `InValue`. When the value has been updated, a notification is sent to all subscribers (if they exist).
```cpp
void SetTypeParameterValue( const FName& InParameterName, 
							type InValue);
```

*3.* Returns a parameter value (returns default value if no such parameter is found).
```cpp
type GetTypeParameterValue(const FName& InParameterName);
```

*4.* Returns `true` if the specified parameter is registered, `false` - otherwise.
```cpp
bool IsTypeParameterValid(const FName& InParameterName);
```

---
#### Subscriptions

In order to subscribe to parameter one can use one of the two following methods:

**1. Explicit subscription** using the following method:
```c++
void SubscribeToParameter(  FName ParameterName, 
							UObject* Subscriber, 
							FName NotificationFunctionName);
```

**2. Subscription during parameter registration**
`RegisterTypeParameter` method provides a similar interface to auto subscribe to the registered parameter.

*NOTE: When a new `UOBJECT` subscribes to a parameter, a on-value-changed notification is immediately sent to all subscribers of this parameter.*

---
#### Notification Function Prototype

Any function, whose name is passed as `NotificationFunctionName` MUST have the following prototype:
```cpp
UFUNCTION() // !!IMPORTANT!!
void OnParameterValueChanged(const FName& ParameterName);
```

*IMPORTANT: The function must be marked as a UFUNCTION. Notifications will not work otherwise, and due to the way it is implemented in the Engine there will be NO error messages, suggesting it.*

---

### Internals

Internally parameters are stored in `TMap` type storages:

```c++
TMap<FName, bool> BoolParameterStorage = { {"Default", false} };
TMap<FName, int32> IntParameterStorage = { {"Default", 0} };
TMap<FName, float> FloatParameterStorage = { {"Default", 0.f} };
```

Each storage contains a `"Default"` value that is used when accessing a non-registered parameter.

Templated methods are used to access/modify parameter storages:

```cpp
// Sets <T> Parameter in given Map:
template< typename T >
void SetParameterValue( TMap<FName, T>& ParameterStorage, 
						const FName& ParameterName, 
						const T& Value);

// Gets <T> Parameter from given Map
template< typename T >
const T& GetParameterValue( TMap<FName, T>& ParameterStorage, 
							const FName& ParameterName);

// Checks if the specified parameter exists and creates it if not
// Can also register a subsciption to the parameter
template< typename T>
void RegisterParameter( TMap<FName, T>& ParameterStorage, 
						const FName& ParameterName, 
						const T& DefaultValue,
						bool AutoSubscribe, 
						UObject* Subscriber, 
						FName NotificationFunctionName);
```

These templated methods are then wrapped by `UFUNCTION` methods to provide a general blueprint-friendly interface.

Subscriptions are stored as `Delegates`
```cpp
// Fires off when custom MPAS_Handler parameter value is changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementParameterValueChanged, FName, InParameterName);
```
inside of a shared storage (shared between parameters of all types)
```cpp
TMap<FName, FOnMovementParameterValueChanged> ParameterSubscriptions;
```

`SendParameterNotifications` method is used to send update notifications to all subscribers of the specified parameter. If the specified parameter does not exist - nothing happens.
```cpp
void SendParameterNotifications(const FName& InParamterName);
```

---
### Debugging

To make debugging of parameter values easier the following method is provided:
```cpp
void GetAllParameterNames(TArray<FName>& BoolParameters, TArray<FName>& IntParameters, TArray<FName>& FloatParameters);
```
It returns arrays of names of every parameter of the specified type.