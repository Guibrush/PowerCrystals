// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PCGameplayAbility.h"
#include "PCAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class POWERCRYSTALS_API UPCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool ActivateAbility(FGameplayTag InputActionTag, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	void CancelCurrentAbility();

	UFUNCTION(BlueprintNativeEvent)
	void AbilityEnded(UPCGameplayAbility* Ability);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, TSubclassOf<UPCGameplayAbility>> UnitAbilities;

	UPROPERTY(BlueprintReadWrite)
	FGameplayAbilityTargetDataHandle CurrentTargetData;

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UPCGameplayAbility> CurrentAbility;
	
};
