// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DynamicGates.generated.h"


UENUM(BlueprintType)
enum class ESCM_DynamicGateRule : uint8
{
	And UMETA(DisplayName = "And"),
	Or UMETA(DisplayName = "Or")
};

USTRUCT(BlueprintType)
struct FSCM_DynamicGate
{
	GENERATED_BODY()

	// Rule that is used to combine gate values to calculate the resulting value
	UPROPERTY()
	ESCM_DynamicGateRule Rule;

	// Named gate values
	UPROPERTY()
	TMap<FName, bool> GateValues;

	// Default value used when the gate does not have any named gate values
	bool DefaultValue; 

	// Cached resulting value, only calculated when the gate values are updated
	UPROPERTY()
	bool CachedResultingValue;

	// Constructor
	FSCM_DynamicGate(ESCM_DynamicGateRule InRule = ESCM_DynamicGateRule::And, bool InDefaultValue = true): 
		Rule(InRule), DefaultValue(InDefaultValue), CachedResultingValue(InDefaultValue) {}

protected:
	// Resultes and caches the resulting value
	void RecalculateResultingValue();

public:
	
	// Updates gate's rule (Rule that is used to combine gate values to calculate the resulting value)
	void SetGateRule(ESCM_DynamicGateRule NewRule);

	// Adds/Updates a named gate value
	void SetNamedGateValue(const FName& Name, bool Value);

	// Removes a named gate value
	void RemoveNamedGateValue(const FName& Name);

	// Returns a pointer to the named gate value, nullptr if not found
	bool* GetNamedGateValue(const FName& Name) { return GateValues.Find(Name); }

	// Returns the resulting gate value, calculated based on the named gate values and the rule
	bool GetResultingGateValue() { return CachedResultingValue; }
};


// Paste this interface into the class you want to store dynamic gates
/*
	// DYNAMIC GATES
protected:

	// Map of dynamic gates
	TMap<FName, FSCM_DynamicGate> DynamicGates;

public:

	// Registers a new dynamic gate, return true if succeded. If a gate with the specified name already exists return "false"
	UFUNCTION(BlueprintCallable, Category = "CLASS|DynamicGates")
	bool RegisterDynamicGate(const FName& GateName, ESCM_DynamicGateRule Rule, bool DefaultValue) 
	{ 
		if (DynamicGates.Contains(GateName)) 
			return false;
		DynamicGates.Add(GateName, FSCM_DynamicGate(Rule, DefaultValue));
		return true;
	}

	// Returns the resulting value of the dynamic gate, "false" if not found
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CLASS|DynamicGates")
	bool GetDynamicGateValue(const FName& GateName)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		if (GateP)
			return GateP->GetResultingGateValue();
		return false;
	}


	// Returns the rule of the specified dynamic gate (Rule that is used to combine gate values to calculate the resulting value)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CLASS|DynamicGates")
	ESCM_DynamicGateRule GetDynamicGateRule(const FName& GateName)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		check(GateP);
		return GateP->Rule;
	}

	// Updates gate's rule (Rule that is used to combine gate values to calculate the resulting value)
	UFUNCTION(BlueprintCallable, Category = "CLASS|DynamicGates")
	void SetDynamicGateRule(const FName& GateName, ESCM_DynamicGateRule NewRule)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		check(GateP);
		GateP->SetGateRule(NewRule);
	}


	// Adds/Updates a named value in the dynamic gate
	UFUNCTION(BlueprintCallable, Category = "CLASS|DynamicGates")
	void SetDynamicGateNamedValue(const FName& GateName, const FName& ValueName, bool Value)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		check(GateP);
		GateP->SetNamedGateValue(ValueName, Value);
	}

	// Removes a named value from the dynamic gate
	UFUNCTION(BlueprintCallable, Category = "CLASS|DynamicGates")
	void RemoveDynamicGateNamedValue(const FName& GateName, const FName& ValueName)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		check(GateP);
		GateP->RemoveNamedGateValue(ValueName);
	}

	// Returns the named value from the dynamic gate
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CLASS|DynamicGates")
	bool GetDynamicGateNamedValue(const FName& GateName, const FName& ValueName)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		check(GateP);
		bool* ValueP = GateP->GetNamedGateValue(ValueName);
		check(ValueP);
		return *ValueP;
	}

	// DEBUG
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CLASS|DynamicGates|Debug")
	void GetAllDynamicGateNames(TArray<FName>& OutNames) { DynamicGates.GetKeys(OutNames); }

	// DEBUG
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CLASS|DynamicGates|Debug")
	void GetDynamicGateValueNames(const FName& GateName, TArray<FName>& OutNames)
	{
		FSCM_DynamicGate* GateP = DynamicGates.Find(GateName);
		if (GateP)
		{
			GateP->GateValues.GetKeys(OutNames);
		}
	}
*/



/**
 * 
 */
//UCLASS()
//class SCARLETCHARACTERMOVEMENT_API UDynamicGates : public UBlueprintFunctionLibrary
//{
//	GENERATED_BODY()
//	
//};
