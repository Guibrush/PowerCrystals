// Fill out your copyright notice in the Description page of Project Settings.


#include "PCTaskSystemComponent.h"
#include "PCTask.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPCTaskSystemComponent::UPCTaskSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UPCTaskSystemComponent::BeginPlay()
{
	Super::BeginPlay();	
}


// Called every frame
void UPCTaskSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ActiveTasks.Num() > 0)
	{
		UPCTask* CurrentTask = ActiveTasks[0];
		if (CurrentTask)
		{
			if (CurrentTask->GetTaskState() == ETaskState::Queued)
			{
				if (CurrentTask->ActivateTask())
				{
					CurrentTask->CurrentRemainingTime -= DeltaTime;
				}
			}
			else if (CurrentTask->GetTaskState() == ETaskState::Activated)
			{
				CurrentTask->CurrentRemainingTime -= DeltaTime;
			}

			if (CurrentTask->CurrentRemainingTime <= 0.0f && CurrentTask->GetTaskState() == ETaskState::Activated)
			{
				CurrentTask->CurrentRemainingTime = 0.0f;
				if (CurrentTask->CompleteTask())
				{
					ActiveTasks.RemoveAt(0);
					OnActiveTasksChanged.Broadcast();
				}
			}
		}
	}
}

void UPCTaskSystemComponent::AddNewTask(TSubclassOf<UPCTask> NewTask)
{
	if (NewTask)
	{
		UPCTask* InstancedTask = NewObject<UPCTask>(GetOwner(), NewTask);
		if (InstancedTask)
		{
			InstancedTask->Owner = GetOwner();
			if (InstancedTask->QueueTask())
			{
				ActiveTasks.Add(InstancedTask);
				OnActiveTasksChanged.Broadcast();
			}
		}
	}
}

bool UPCTaskSystemComponent::PauseCurrentTask()
{
	AActor* Owner = GetOwner<AActor>();
	if (!Owner)
	{
		return false;
	}

	if (!Owner->HasAuthority())
	{
		ServerSetPauseCurrentTask(true);
		return true;
	}
	else
	{
		if (ActiveTasks.Num() > 0)
		{
			UPCTask* CurrentTask = ActiveTasks[0];
			if (CurrentTask && CurrentTask->GetTaskState() == ETaskState::Activated)
			{
				return CurrentTask->PauseTask();
			}
		}
	}

	return false;
}

bool UPCTaskSystemComponent::UnPauseCurrentTask()
{
	AActor* Owner = GetOwner<AActor>();
	if (!Owner)
	{
		return false;
	}

	if (!Owner->HasAuthority())
	{
		ServerSetPauseCurrentTask(false);
		return true;
	}
	else
	{
		if (ActiveTasks.Num() > 0)
		{
			UPCTask* CurrentTask = ActiveTasks[0];
			if (CurrentTask && CurrentTask->GetTaskState() == ETaskState::Paused)
			{
				return CurrentTask->ActivateTask();
			}
		}
	}

	return false;
}

bool UPCTaskSystemComponent::CancelTask(int32 TaskIndex)
{
	AActor* Owner = GetOwner<AActor>();
	if (!Owner)
	{
		return false;
	}

	if (!Owner->HasAuthority())
	{
		ServerCancelTask(TaskIndex);
		return true;
	}
	else
	{
		if (ActiveTasks.Num() > TaskIndex)
		{
			UPCTask* TaskToCancel = ActiveTasks[TaskIndex];
			if (TaskToCancel &&
				(TaskToCancel->GetTaskState() == ETaskState::Queued ||
					TaskToCancel->GetTaskState() == ETaskState::Activated ||
					TaskToCancel->GetTaskState() == ETaskState::Paused))
			{
				if (TaskToCancel->CancelTask())
				{
					ActiveTasks.RemoveAt(TaskIndex);
					return true;
				}
			}
		}
	}

	return false;
}

void UPCTaskSystemComponent::ServerCancelTask_Implementation(int32 TaskIndex)
{
	CancelTask(TaskIndex);
}

void UPCTaskSystemComponent::ServerSetPauseCurrentTask_Implementation(bool NewPause)
{
	if (NewPause)
	{
		PauseCurrentTask();
	}
	else
	{
		UnPauseCurrentTask();
	}
}

TArray<UPCTask*> UPCTaskSystemComponent::GetActiveTasks() const
{
	return ActiveTasks;
}

void UPCTaskSystemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPCTaskSystemComponent, ActiveTasks);
}
