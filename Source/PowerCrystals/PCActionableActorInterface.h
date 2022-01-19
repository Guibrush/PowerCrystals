// Copyright 2019-2020 Alberto & co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "PCActionableActorInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UPCActionableActorInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class POWERCRYSTALS_API IPCActionableActorInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	virtual bool ExecuteAbility(FGameplayTag AbilityTag, FHitResult Hit) = 0;
	virtual void ActorSelected() = 0;
	virtual void ActorDeselected() = 0;
	virtual void SpawnPlayerUnit(TSubclassOf<class APCUnit> UnitBlueprint) = 0;
	virtual bool IsAlive() = 0;
	virtual class UPCAbilitySystemComponent* GetAbilitySystem() = 0;
	virtual FGameplayTag GetTeam() = 0;
	virtual FGameplayTag GetFaction() = 0;
	virtual class APCPlayerController* GetControllerOwner() = 0;
};
