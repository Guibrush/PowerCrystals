// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "PCCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class POWERCRYSTALS_API UPCCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:

	UFUNCTION(exec, Category = "PowerCrystals.ActionableActors")
	void DealDamageToSelectedActors();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayAbility> DealDamageAbility;
	
};
