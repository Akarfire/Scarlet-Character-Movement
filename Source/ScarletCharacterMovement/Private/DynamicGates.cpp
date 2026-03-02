// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicGates.h"

void FSCM_DynamicGate::RecalculateResultingValue()
{
	if (GateValues.Num() == 0)
	{
		CachedResultingValue = DefaultValue;
		return;
	}

	switch (Rule)
	{
	case ESCM_DynamicGateRule::And:
		CachedResultingValue = true;
		for (auto& GateValueEntry : GateValues)
			CachedResultingValue = CachedResultingValue && GateValueEntry.Value;
		break;

	case ESCM_DynamicGateRule::Or:
		CachedResultingValue = false;
		for (auto& GateValueEntry : GateValues)
			CachedResultingValue = CachedResultingValue || GateValueEntry.Value;
		break;
	}
}

void FSCM_DynamicGate::SetGateRule(ESCM_DynamicGateRule NewRule)
{
	Rule = NewRule;
	RecalculateResultingValue();
}

void FSCM_DynamicGate::SetNamedGateValue(const FName& Name, bool Value)
{
	GateValues.Add(Name, Value);
	RecalculateResultingValue();
}

void FSCM_DynamicGate::RemoveNamedGateValue(const FName& Name)
{
	GateValues.Remove(Name);
	RecalculateResultingValue();
}
