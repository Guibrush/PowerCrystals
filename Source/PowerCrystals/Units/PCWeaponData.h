// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PCWeaponData.generated.h"

/**
 * 
 */
UCLASS()
class POWERCRYSTALS_API UPCWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USkeletalMesh* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttackInputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UPCGameplayAbility> AttackAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* AttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* DeathAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimSequence* IdleAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimSequence* IdleAttackAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimSequence* WalkAnim;
	
};
