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
	FGameplayAbilityTargetDataHandle GetCurrentTargetData() const;

	UFUNCTION(BlueprintPure)
	class APCUnit* GetOwningUnit();

	UFUNCTION(BlueprintPure)
	class APCPlayerCharacter* GetOwningPlayer();

	UFUNCTION(BlueprintPure)
	class APCPlayerController* GetPlayerController();
	
	UFUNCTION(BlueprintPure)
	class UPCAbilitySystemComponent* GetAbilitySystemComponent();

	UFUNCTION(BlueprintCallable)
	void UnblockInput();

	/** Whether the execution of this ability blocks the input or not. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AbilityConfig)
	bool BlocksInput;

	/** Whether this ability should be shown on the UI or not. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AbilityConfig)
	bool ShowOnUI;

protected:

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
