// Fill out your copyright notice in the Description page of Project Settings.


#include "PCActionableActorComponent.h"
#include "../Units/PCUnit.h"
#include "../Buildings/PCBuilding.h"
#include "../Abilities/PCAbilitySystemComponent.h"
#include "../PCActionableActorInterface.h"
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

	ActionableActor = GetOwner<IPCActionableActorInterface>();
	
}

// Called every frame
void UPCActionableActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPCActionableActorComponent::ActorSelected()
{
	if (ActionableActor)
	{
		ActionableActor->ActorSelected();
	}
}

void UPCActionableActorComponent::ActorDeselected()
{
	if (ActionableActor)
	{
		ActionableActor->ActorDeselected();
	}
}

void UPCActionableActorComponent::HealthChanged(float NewHealth, AActor* Attacker)
{
	HealthChangedMulticast(NewHealth, Attacker);
}

void UPCActionableActorComponent::ActorDied(AActor* Killer)
{
	if (ActionableActor && ActionableActor->GetControllerOwner())
	{
		ActionableActor->GetControllerOwner()->ActionableActorDied(Killer, GetOwner<AActor>());
	}

	ActorDiedMulticast(Killer);
}

void UPCActionableActorComponent::HealthChangedMulticast_Implementation(float NewHealth, AActor* Attacker)
{
	OnHealthChanged.Broadcast(NewHealth, Attacker);
}

void UPCActionableActorComponent::ActorDiedMulticast_Implementation(AActor* Killer)
{
	OnDied.Broadcast(Killer, GetOwner<AActor>());
}

bool UPCActionableActorComponent::IsAlive()
{
	if (ActionableActor)
	{
		return ActionableActor->IsAlive();
	}

	return false;
}

bool UPCActionableActorComponent::IsInPreview()
{
	if (ActionableActor)
	{
		return ActionableActor->IsPreview();
	}

	return false;
}

bool UPCActionableActorComponent::IsInConstruction()
{
	if (ActionableActor)
	{
		return ActionableActor->IsConstruction();
	}

	return false;
}

float UPCActionableActorComponent::GetConstructionTime()
{
	if (ActionableActor)
	{
		return ActionableActor->GetConstructionTime();
	}

	return 0.0f;
}

bool UPCActionableActorComponent::ExecuteAbility(FGameplayTag AbilityTag, FHitResult Hit)
{
	if (ActionableActor)
	{
		return ActionableActor->ExecuteAbility(AbilityTag, Hit);
	}

	return false;
}

void UPCActionableActorComponent::SpawnPlayerUnit(TSubclassOf<APCUnit> UnitBlueprint)
{
	if (ActionableActor)
	{
		ActionableActor->SpawnPlayerUnit(UnitBlueprint);
	}
}

UPCAbilitySystemComponent* UPCActionableActorComponent::GetAbilitySystem()
{
	const IAbilitySystemInterface* ASI = GetOwner<IAbilitySystemInterface>();
	if (ASI)
	{
		return Cast<UPCAbilitySystemComponent>(ASI->GetAbilitySystemComponent());
	}

	return nullptr;
}

FGameplayTag UPCActionableActorComponent::GetTeam()
{
	if (ActionableActor)
	{
		return ActionableActor->GetTeam();
	}

	return FGameplayTag::EmptyTag;
}

FGameplayTag UPCActionableActorComponent::GetFaction()
{
	if (ActionableActor)
	{
		return ActionableActor->GetFaction();
	}

	return FGameplayTag::EmptyTag;
}

APCPlayerController* UPCActionableActorComponent::GetControllerOwner()
{
	if (ActionableActor)
	{
		ActionableActor->GetControllerOwner();
	}

	return nullptr;
}
