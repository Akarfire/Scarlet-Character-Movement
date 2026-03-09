### Concept

Dynamic Gates are structures that contain a set of named Boolean values that are combined using a *Rule* to get a resulting Boolean value. These gates are used for dynamic state-transition conditions, that can be accessed and modified from any layer of the Movement State Machine stack.

*Rules* by which the resulting dynamic gate value is calculated are described as an `enum`:
```cpp
enum class ESCM_DynamicGateRule : uint8
{
    And,
    Or
};
```

* `And` rule: resulting value is `true` only if every named value inside the dynamic gate is `true`;
* `Or` rule: resulting value is `true` if at least one named value is `true`.

Dynamic gates have *Default Values* that are used in cases, when no named values are present inside the dynamic gate.

### Usage pattern

1. Register a dynamic gate;
2. Use the resulting value for state transitions by accessing it via `GetDynamicGateValue`;
3. Add/Modify named values to affect the resulting value.

---
### Interface

Dynamic gates can be accessed/modified using the following interface:

*1.* Registers a new dynamic gate, returns true if succeeded. If a gate with the specified name already exists return "false".
```cpp
bool RegisterDynamicGate(const FName& GateName, ESCM_DynamicGateRule Rule, bool DefaultValue);
```

*2.* Whether a dynamic gate with the specified name exists or not.
```cpp
bool IsDynamicGateValid(const FName& GateName);
```

*3.* Returns the resulting value of the dynamic gate, "false" if such gate does not exist.
```cpp
bool GetDynamicGateValue(const FName& GateName);
```

*4.* Returns the rule of the specified dynamic gate (Rule that is used to combine gate values to calculate the resulting value). Throws an error if the specified gate does not exist.
```cpp
ESCM_DynamicGateRule GetDynamicGateRule(const FName& GateName);
```

*5.* Updates gate's rule (Rule that is used to combine gate values to calculate the resulting value). Throws an error if the specified gate does not exist.
```cpp
void SetDynamicGateRule(const FName& GateName, ESCM_DynamicGateRule NewRule);
```

*6.* Adds/Updates a named value in the dynamic gate. Throws an error if the specified gate does not exist.
```cpp
void SetDynamicGateNamedValue(const FName& GateName, const FName& ValueName, bool Value);
```

*7.* Removes a named value from the dynamic gate. Throws an error if the specified gate does not exist.
```cpp
void RemoveDynamicGateNamedValue(const FName& GateName, const FName& ValueName);
```

*8.* Returns the named value from the dynamic gate. Throws an error if the specified gate or named value does not exist.
```cpp
bool GetDynamicGateNamedValue(const FName& GateName, const FName& ValueName);
```

---
### Internals

*Include: `DynamicGates.h`*

Internally dynamic gates are represented by `FSCM_DynamicGate` structures:

```cpp
struct FSCM_DynamicGate
{

	// Rule that is used to combine gate values to calculate the resulting value
	ESCM_DynamicGateRule Rule;

	// Named gate values
	TMap<FName, bool> GateValues;

    // Default value used when the gate does not have any named gate values
    bool DefaultValue;

    // Cached resulting value, only calculated when the gate values are updated
    bool CachedResultingValue;

    // Constructor
    FSCM_DynamicGate( ESCM_DynamicGateRule InRule = ESCM_DynamicGateRule::And, 
					  bool InDefaultValue = true);
  
protected:
	void RecalculateResultingValue();

  
public:

    // Updates gate's rule (Rule that is used to combine gate values to calculate the resulting value)
    void SetGateRule(ESCM_DynamicGateRule NewRule);

    // Adds/Updates a named gate value
    void SetNamedGateValue(const FName& Name, bool Value);

    // Removes a named gate value
    void RemoveNamedGateValue(const FName& Name);

    // Returns a pointer to the named gate value, nullptr if not found
    bool* GetNamedGateValue(const FName& Name);

    // Returns the resulting gate value, calculated based on the named gate values and the rule
    bool GetResultingGateValue();
};
```

Resulting value is recalculated and cached only when any named value is added/modified.

Dynamic gates are stored in a `TMap`:
```cpp
TMap<FName, FSCM_DynamicGate> DynamicGates;
```

---
### Debugging

To simplify debugging of dynamic gate values the following methods are provided:

```cpp
void GetAllDynamicGateNames(TArray<FName>& OutNames);

void GetDynamicGateValueNames(const FName& GateName, TArray<FName>& OutNames);
```