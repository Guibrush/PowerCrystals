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
		TArray<AActor*> ActorsDamageApplied;
		bool AllActorsDamaged = false;
		int32 NumActorsSelected = PCController->SelectedActors.Num();
		while (!AllActorsDamaged)
		{
			for (int32 i = 0; i < NumActorsSelected; i++)
			{
				if ((i < PCController->SelectedActors.Num()) && !ActorsDamageApplied.Contains(PCController->SelectedActors[i]))
				{
					IAbilitySystemInterface* AbilitySystemActor = Cast<IAbilitySystemInterface>(PCController->SelectedActors[i]);
					if (AbilitySystemActor && AbilitySystemActor->GetAbilitySystemComponent())
					{
						ActorsDamageApplied.Add(PCController->SelectedActors[i]);

						FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(DealDamageAbility, 1, 0);
						AbilitySystemActor->GetAbilitySystemComponent()->GiveAbilityAndActivateOnce(AbilitySpec);
					}
				}
			}

			AllActorsDamaged = ActorsDamageApplied.Num() == NumActorsSelected;
		}
	}
}
