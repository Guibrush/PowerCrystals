// Fill out your copyright notice in the Description page of Project Settings.


#include "PCAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "../Player/PCPlayerController.h"
#include "../Components/PCActionableActorComponent.h"

void UPCAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerActor() && GetOwnerActor()->HasAuthority())
	{
		for (TPair<FGameplayTag, TSubclassOf<UPCGameplayAbility>> AbilityDesc : Abilities)
		{
			GiveAbility(FGameplayAbilitySpec(AbilityDesc.Value.Get(), 1, 0));
		}
	}
}

void UPCAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
}

bool UPCAbilitySystemComponent::ActivateAbility(FGameplayTag InputActionTag, FHitResult Hit, APCPlayerController* PlayerController)
{
	TSubclassOf<UPCGameplayAbility> Ability = Abilities.Find(InputActionTag)->Get();
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

void UPCAbilitySystemComponent::CancelCurrentAbility(APCPlayerController* PlayerController)
{
	if (CurrentAbility)
	{
		CancelAbility(CurrentAbility.GetDefaultObject());

		CurrentAbility = nullptr;
		CurrentTargetData = nullptr;
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
