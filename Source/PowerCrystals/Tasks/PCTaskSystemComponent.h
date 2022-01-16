// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PCTask.h"
#include "PCTaskSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActiveTasksChanged);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POWERCRYSTALS_API UPCTaskSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPCTaskSystemComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void AddNewTask(TSubclassOf<class UPCTask> NewTask);

	UFUNCTION(BlueprintPure)
	TArray<UPCTask*> GetActiveTasks() const;

	UPROPERTY(BlueprintAssignable)
	FOnActiveTasksChanged OnActiveTasksChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<UPCTask*> ActiveTasks;

private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
		
};
