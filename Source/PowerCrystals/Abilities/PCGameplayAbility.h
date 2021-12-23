// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PCGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class POWERCRYSTALS_API UPCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	FGameplayAbilityTargetDataHandle GetCurrentTargetData();

	UFUNCTION(BlueprintPure)
	class APCUnit* GetOwningUnit();
	
	UFUNCTION(BlueprintPure)
	class UPCAbilitySystemComponent* GetAbilitySystemComponent();

protected:

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
