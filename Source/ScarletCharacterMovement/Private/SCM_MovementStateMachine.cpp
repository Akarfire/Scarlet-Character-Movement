// Fill out your copyright notice in the Description page of Project Settings.


#include "SCM_MovementStateMachine.h"
#include "SCM_MovementStateBase.h"


// Called every time the state machine is updated (after the main update) (to be overriden)
void USCM_MovementStateMachine::OnUpdateStateMachine_Implementation(float DeltaTime)
{
	USCM_MovementStateBase* MovementState = Cast<USCM_MovementStateBase>(GetState(GetActiveState()));
	if (MovementState)
		GetScarletMovement()->SetActiveState(MovementState);
}

// CALLED BY ScarletMovementComponent. Call SetupParameters on the states and triggers OnSetupParameters
void USCM_MovementStateMachine::SetupParameters()
{
	OnSetupParameters();

	for (auto& State : GetStates())
	{
		USCM_MovementStateBase* MovementState = Cast<USCM_MovementStateBase>(State.Value);
		if (MovementState)
			MovementState->SetupParameters();
	}
}
