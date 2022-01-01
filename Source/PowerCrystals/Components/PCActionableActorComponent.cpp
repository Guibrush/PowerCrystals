// Fill out your copyright notice in the Description page of Project Settings.


#include "PCActionableActorComponent.h"
#include "../Units/PCUnit.h"
#include "../Buildings/PCBuilding.h"

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
