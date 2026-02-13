// Fill out your copyright notice in the Description page of Project Settings.


#include "SCM_MovementStateBase.h"

void USCM_MovementStateBase::STATEMACHINE_OnSetStateMachine_Implementation()
{
	USCM_MovementStateMachine* MovementStateMachine = Cast<USCM_MovementStateMachine>(GetStateMachine());
	if (MovementStateMachine)
	{
		ScarletMovement = MovementStateMachine->GetScarletMovement();
	}
}
