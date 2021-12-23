// Fill out your copyright notice in the Description page of Project Settings.


#include "PCAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UPCAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerActor() && GetOwnerActor()->HasAuthority())
	{
		for (TPair<FGameplayTag, TSubclassOf<UPCGameplayAbility>> AbilityDesc : UnitAbilities)
		{
			GiveAbility(FGameplayAbilitySpec(AbilityDesc.Value.Get(), 1, 0));
		}
	}
}

bool UPCAbilitySystemComponent::ActivateAbility(FGameplayTag InputActionTag, FHitResult Hit)
{
	TSubclassOf<UPCGameplayAbility> Ability = UnitAbilities.Find(InputActionTag)->Get();
	if (Ability)
	{
		CurrentTargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Hit);
		if (TryActivateAbilityByClass(Ability))
		{
			CurrentAbility = Ability;
			return true;
		}
	}

	CurrentAbility = nullptr;
	CurrentTargetData = nullptr;
	return false;
}

void UPCAbilitySystemComponent::CancelCurrentAbility()
{
	if (CurrentAbility)
	{
		CancelAbility(CurrentAbility.GetDefaultObject());
	}
}

void UPCAbilitySystemComponent::AbilityEnded_Implementation(UPCGameplayAbility* Ability)
{
	if (CurrentAbility.GetDefaultObject() == Ability)
	{
		CurrentAbility = nullptr;
		CurrentTargetData = nullptr;
	}
}
