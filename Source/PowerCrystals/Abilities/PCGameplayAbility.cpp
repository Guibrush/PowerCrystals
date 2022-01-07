// Fill out your copyright notice in the Description page of Project Settings.


#include "PCGameplayAbility.h"
#include "../Units/PCUnit.h"
#include "../Player/PCPlayerCharacter.h"
#include "../Player/PCPlayerController.h"
#include "PCAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

FGameplayAbilityTargetDataHandle UPCGameplayAbility::GetCurrentTargetData() const
{
	APCUnit* OwningUnit = Cast<APCUnit>(GetOwningActorFromActorInfo());
	if (OwningUnit)
	{
		return OwningUnit->GetAbilitySystem()->CurrentTargetData;
	}

	return nullptr;
}

APCUnit* UPCGameplayAbility::GetOwningUnit()
{
	return Cast<APCUnit>(GetOwningActorFromActorInfo());
}

APCPlayerCharacter* UPCGameplayAbility::GetOwningPlayer()
{
	return Cast<APCPlayerCharacter>(GetOwningActorFromActorInfo());
}

APCPlayerController* UPCGameplayAbility::GetPlayerController()
{
	if (GetOwningUnit())
	{
		return GetOwningUnit()->PlayerOwner;
	}

	if (GetOwningPlayer())
	{
		return GetOwningPlayer()->GetController<APCPlayerController>();
	}

	return nullptr;
}

class UPCAbilitySystemComponent* UPCGameplayAbility::GetAbilitySystemComponent()
{
	return Cast<UPCAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

void UPCGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UPCAbilitySystemComponent* AbilitySystem = Cast<UPCAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (AbilitySystem)
	{
		AbilitySystem->AbilityEnded(this);
	}
}
