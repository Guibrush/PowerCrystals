// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PCTask.generated.h"

UENUM()
enum class ETaskState : uint8
{
	Inactive,
	Queued,
	Activated,
	Paused,
	Completed,
	Cancelled
};

/**
 * 
 */
UCLASS(Blueprintable)
class POWERCRYSTALS_API UPCTask : public UObject
{
	GENERATED_BODY()

public:

	UPCTask();

	virtual bool IsSupportedForNetworking() const override { return true; }

	bool QueueTask();

	bool ActivateTask();

	bool PauseTask();

	bool CompleteTask();

	bool CancelTask();

	bool CanQueueTask();

	bool CanActivateTask();

	bool CanPauseTask();

	bool CanCompleteTask();

	bool CanCancelTask();

	UFUNCTION(BlueprintNativeEvent, Category = PCTask, DisplayName = "CanQueueTask", meta = (ScriptName = "CanQueueTask"))
	bool BP_CanQueueTask();

	UFUNCTION(BlueprintNativeEvent, Category = PCTask, DisplayName = "CanActivateTask", meta = (ScriptName = "CanActivateTask"))
	bool BP_CanActivateTask();

	UFUNCTION(BlueprintNativeEvent, Category = PCTask, DisplayName = "CanPauseTask", meta = (ScriptName = "CanPauseTask"))
	bool BP_CanPauseTask();

	UFUNCTION(BlueprintNativeEvent, Category = PCTask, DisplayName = "CanCompleteTask", meta = (ScriptName = "CanCompleteTask"))
	bool BP_CanCompleteTask();

	UFUNCTION(BlueprintNativeEvent, Category = PCTask, DisplayName = "CanCancelTask", meta = (ScriptName = "CanCancelTask"))
	bool BP_CanCancelTask();

	UFUNCTION(BlueprintImplementableEvent, Category = PCTask, DisplayName = "QueueTask", meta = (ScriptName = "QueueTask"))
	void BP_QueueTask();

	UFUNCTION(BlueprintImplementableEvent, Category = PCTask, DisplayName = "ActivateTask", meta = (ScriptName = "ActivateTask"))
	void BP_ActivateTask();

	UFUNCTION(BlueprintImplementableEvent, Category = PCTask, DisplayName = "PauseTask", meta = (ScriptName = "PauseTask"))
	void BP_PauseTask();

	UFUNCTION(BlueprintImplementableEvent, Category = PCTask, DisplayName = "CompleteTask", meta = (ScriptName = "CompleteTask"))
	void BP_CompleteTask();

	UFUNCTION(BlueprintImplementableEvent, Category = PCTask, DisplayName = "CancelTask", meta = (ScriptName = "CancelTask"))
	void BP_CancelTask();

	UFUNCTION(BlueprintPure)
	ETaskState GetTaskState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TaskCompletionTime;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float CurrentRemainingTime;

	UPROPERTY(BlueprintReadOnly, Replicated)
	class AActor* Owner;

protected:

	UPROPERTY(BlueprintReadOnly, Replicated)
	ETaskState TaskState;

private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
};
