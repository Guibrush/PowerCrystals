// Fill out your copyright notice in the Description page of Project Settings.


#include "PCTask.h"
#include "Net/UnrealNetwork.h"


UPCTask::UPCTask() : Super()
{
	TaskState = ETaskState::Inactive;
}

bool UPCTask::QueueTask()
{
	if (CanQueueTask())
	{
		// Other necessary checks to ensure that this task has been properly queued
		// before calling the blueprint code.
		
		BP_QueueTask();

		TaskState = ETaskState::Queued;

		return true;
	}

	return false;
}

bool UPCTask::ActivateTask()
{
	if (CanActivateTask())
	{
		// Other necessary checks to ensure that this task has been properly activated
		// before calling the blueprint code.
		CurrentRemainingTime = TaskCompletionTime;

		BP_ActivateTask();

		TaskState = ETaskState::Activated;

		return true;
	}

	return false;
}

bool UPCTask::CompleteTask()
{
	if (CanCompleteTask())
	{
		// Other necessary checks to ensure that this task has been properly completed
		// before calling the blueprint code.

		BP_CompleteTask();

		TaskState = ETaskState::Completed;

		return true;
	}

	return false;
}

bool UPCTask::CanQueueTask()
{
	// Other checks before calling the blueprint code.

	return BP_CanQueueTask();
}

bool UPCTask::CanActivateTask()
{
	// Other checks before calling the blueprint code.

	return BP_CanActivateTask();
}

bool UPCTask::CanCompleteTask()
{
	// Other checks before calling the blueprint code.

	return BP_CanCompleteTask();
}

bool UPCTask::BP_CanQueueTask_Implementation()
{
	// This function is meant to be implemented in blueprints.
	// The only reason why this exist is because we need to
	// return true by default in case the blueprint doesn't
	// want to implement it.
	return true;
}

bool UPCTask::BP_CanActivateTask_Implementation()
{
	// This function is meant to be implemented in blueprints.
	// The only reason why this exist is because we need to
	// return true by default in case the blueprint doesn't
	// want to implement it.
	return true;
}

bool UPCTask::BP_CanCompleteTask_Implementation()
{
	// This function is meant to be implemented in blueprints.
	// The only reason why this exist is because we need to
	// return true by default in case the blueprint doesn't
	// want to implement it.
	return true;
}

ETaskState UPCTask::GetTaskState()
{
	return TaskState;
}

void UPCTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPCTask, CurrentRemainingTime);
	DOREPLIFETIME(UPCTask, TaskState);
	DOREPLIFETIME(UPCTask, Owner);
}
