// Fill out your copyright notice in the Description page of Project Settings.


#include "SCM_LowerLevelState.h"
#include "SCM_MovementStateMachine.h"

// Called when the state is made active
void USCM_LowerLayerState::EnterState_Implementation()
{
	if (EmbeddedStateMachine)
	{
		USCM_MovementStateMachine* MovementStateMachine = Cast<USCM_MovementStateMachine>(EmbeddedStateMachine);
		if (MovementStateMachine)
			MovementStateMachine->OnLayerEntered();
	}

	//Super::EnterState();
	if (!EmbeddedStateMachine) return;

	USSM_StateBase* ActiveState = EmbeddedStateMachine->GetState(EmbeddedStateMachine->GetActiveState());
	if (ActiveState)
		ActiveState->EnterState();
}

// Called once the active state is changed to a different one
void USCM_LowerLayerState::ExitState_Implementation()
{
	if (EmbeddedStateMachine)
	{
		USCM_MovementStateMachine* MovementStateMachine = Cast<USCM_MovementStateMachine>(EmbeddedStateMachine);
		if (MovementStateMachine)
			MovementStateMachine->OnLayerExit();
	}

	//Super::ExitState();
	if (!EmbeddedStateMachine) return;

	USSM_StateBase* ActiveState = EmbeddedStateMachine->GetState(EmbeddedStateMachine->GetActiveState());
	if (ActiveState)
		ActiveState->ExitState();
}
