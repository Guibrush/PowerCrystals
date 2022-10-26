// Fill out your copyright notice in the Description page of Project Settings.


#include "PCTechTreeSystemComponent.h"
#include "PCTech.h"
#include "../Player/PCPlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPCTechTreeSystemComponent::UPCTechTreeSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UPCTechTreeSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPCTechTreeSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPCTechTreeSystemComponent::AddNewTech(TSubclassOf<UPCTech> NewTech)
{
	APCPlayerCharacter* PlayerOwner = Cast<APCPlayerCharacter>(GetOwner());

	if (PlayerOwner && NewTech)
	{
		UPCTech* InstancedTech = NewObject<UPCTech>(this, NewTech);
		if (InstancedTech)
		{
			InstancedTech->Owner = GetOwner();

			for (AActor* ActionableActor : PlayerOwner->GetPlayerActionableActors())
			{
				InstancedTech->ApplyTech(ActionableActor);
			}

			ActiveTechs.Add(InstancedTech);
		}
	}
}

void UPCTechTreeSystemComponent::ApplyTechTree(AActor* ActorToApply)
{
	if (ActorToApply)
	{
		for (UPCTech* Tech : ActiveTechs)
		{
			Tech->ApplyTech(ActorToApply);
		}
	}
}

TArray<UPCTech*> UPCTechTreeSystemComponent::GetActiveTechs() const
{
	return ActiveTechs;
}

void UPCTechTreeSystemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPCTechTreeSystemComponent, ActiveTechs);
}
