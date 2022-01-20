// Fill out your copyright notice in the Description page of Project Settings.


#include "PCCheatManager.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "../Player/PCPlayerController.h"


void UPCCheatManager::DealDamageToSelectedActors()
{
	APCPlayerController* PCController = Cast<APCPlayerController>(GetOuterAPlayerController());
	if (PCController)
	{
		for (AActor* SelectedActor : PCController->SelectedActors)
		{
			IAbilitySystemInterface* AbilitySystemActor = Cast<IAbilitySystemInterface>(SelectedActor);
			if (AbilitySystemActor && AbilitySystemActor->GetAbilitySystemComponent())
			{
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(DealDamageAbility, 1, 0);
				AbilitySystemActor->GetAbilitySystemComponent()->GiveAbilityAndActivateOnce(AbilitySpec);
			}
		}
	}
}
