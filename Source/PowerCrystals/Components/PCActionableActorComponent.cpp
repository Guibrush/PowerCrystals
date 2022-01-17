// Fill out your copyright notice in the Description page of Project Settings.


#include "PCActionableActorComponent.h"
#include "../Units/PCUnit.h"
#include "../Buildings/PCBuilding.h"
#include "../Abilities/PCAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

// Sets default values for this component's properties
UPCActionableActorComponent::UPCActionableActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UPCActionableActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UPCActionableActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPCActionableActorComponent::InitComponent(bool NewIsUnit, bool NewIsBuilding)
{
	IsUnit = NewIsUnit;
	IsBuilding = NewIsBuilding;
}

void UPCActionableActorComponent::HealthChanged(float NewHealth, AActor* Attacker)
{
	HealthChangedMulticast(NewHealth, Attacker);
}

void UPCActionableActorComponent::ActorSelected()
{
	if (IsUnit)
	{
		APCUnit* UnitOwner = GetOwner<APCUnit>();
		if (UnitOwner)
		{
			UnitOwner->UnitSelected();
		}
	}
	else if (IsBuilding)
	{
		APCBuilding* BuildingOwner = GetOwner<APCBuilding>();
		if (BuildingOwner)
		{
			BuildingOwner->BuildingSelected();
		}
	}
}

void UPCActionableActorComponent::ActorDeselected()
{
	if (IsUnit)
	{
		APCUnit* UnitOwner = GetOwner<APCUnit>();
		if (UnitOwner)
		{
			UnitOwner->UnitDeselected();
		}
	}
	else if (IsBuilding)
	{
		APCBuilding* BuildingOwner = GetOwner<APCBuilding>();
		if (BuildingOwner)
		{
			BuildingOwner->BuildingDeselected();
		}
	}
}

void UPCActionableActorComponent::HealthChangedMulticast_Implementation(float NewHealth, AActor* Attacker)
{
	OnHealthChanged.Broadcast(NewHealth, Attacker);
}

bool UPCActionableActorComponent::IsAlive()
{
	if (IsUnit)
	{
		APCUnit* UnitOwner = GetOwner<APCUnit>();
		if (UnitOwner)
		{
			return !UnitOwner->IsDead;
		}
	}
	else if (IsBuilding)
	{
		APCBuilding* BuildingOwner = GetOwner<APCBuilding>();
		if (BuildingOwner)
		{
			return !BuildingOwner->IsDestroyed;
		}
	}

	return false;
}

bool UPCActionableActorComponent::ExecuteAbility(FGameplayTag AbilityTag, FHitResult Hit)
{
	if (IsUnit)
	{
		APCUnit* UnitOwner = GetOwner<APCUnit>();
		if (UnitOwner)
		{
			return UnitOwner->ExecuteAbility(AbilityTag, Hit);
		}
	}
	else if (IsBuilding)
	{
		APCBuilding* BuildingOwner = GetOwner<APCBuilding>();
		if (BuildingOwner)
		{
			return BuildingOwner->ExecuteAbility(AbilityTag, Hit);
		}
	}

	return false;
}

void UPCActionableActorComponent::SpawnPlayerUnit(TSubclassOf<APCUnit> UnitBlueprint)
{
	if (IsBuilding)
	{
		APCBuilding* BuildingOwner = GetOwner<APCBuilding>();
		if (BuildingOwner)
		{
			TArray<TSubclassOf<APCUnit>> UnitBlueprints;
			UnitBlueprints.Add(UnitBlueprint);
			BuildingOwner->SpawnPlayerUnits(UnitBlueprints);
		}
	}
}

UPCAbilitySystemComponent* UPCActionableActorComponent::GetAbilitySystem()
{
	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner<AActor>());
	if (ASI)
	{
		return Cast<UPCAbilitySystemComponent>(ASI->GetAbilitySystemComponent());
	}

	return nullptr;
}

FGameplayTag UPCActionableActorComponent::GetTeam()
{
	if (IsUnit)
	{
		APCUnit* UnitOwner = GetOwner<APCUnit>();
		if (UnitOwner)
		{
			return UnitOwner->Team;
		}
	}
	else if (IsBuilding)
	{
		APCBuilding* BuildingOwner = GetOwner<APCBuilding>();
		if (BuildingOwner)
		{
			return BuildingOwner->Team;
		}
	}

	return FGameplayTag::EmptyTag;
}

FGameplayTag UPCActionableActorComponent::GetFaction()
{
	if (IsUnit)
	{
		APCUnit* UnitOwner = GetOwner<APCUnit>();
		if (UnitOwner)
		{
			return UnitOwner->Faction;
		}
	}
	else if (IsBuilding)
	{
		APCBuilding* BuildingOwner = GetOwner<APCBuilding>();
		if (BuildingOwner)
		{
			return BuildingOwner->Faction;
		}
	}

	return FGameplayTag::EmptyTag;
}

APCPlayerController* UPCActionableActorComponent::GetControllerOwner()
{
	if (IsUnit)
	{
		APCUnit* UnitOwner = GetOwner<APCUnit>();
		if (UnitOwner)
		{
			return UnitOwner->PlayerOwner;
		}
	}
	else if (IsBuilding)
	{
		APCBuilding* BuildingOwner = GetOwner<APCBuilding>();
		if (BuildingOwner)
		{
			return BuildingOwner->PlayerOwner;
		}
	}

	return nullptr;
}
