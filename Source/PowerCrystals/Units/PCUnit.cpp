// Fill out your copyright notice in the Description page of Project Settings.


#include "PCUnit.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "../Buildings/PCBuilding.h"
#include "../Components/PCActionableActorComponent.h"

// Sets default values
APCUnit::APCUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystem = CreateDefaultSubobject<UPCAbilitySystemComponent>("AbilitySystem");

	ActionableActorComponent = CreateDefaultSubobject<UPCActionableActorComponent>("ActionableActorComponent");
	ActionableActorComponent->InitComponent(true, false);
	ActionableActorComponent->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void APCUnit::BeginPlay()
{
	Super::BeginPlay();

	USkeletalMesh* MergedMesh = UPCMeshMergeFunctionLibrary::MergeMeshes(MeshMergeParameters);
	if (MergedMesh)
	{
		GetMesh()->SetSkeletalMesh(MergedMesh);
	}

	OnUnitHealthChangedDelegate = FScriptDelegate();
	OnUnitHealthChangedDelegate.BindUFunction(this, "UnitHealthChanged");
	ActionableActorComponent->OnHealthChanged.Add(OnUnitHealthChangedDelegate);

	InitUnit();
}

// Called every frame
void APCUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APCUnit::UnitSelected_Implementation()
{

}

void APCUnit::UnitDeselected_Implementation()
{

}

void APCUnit::UnitHealthChanged_Implementation(float NewValue, AActor* Attacker)
{
	if (!IsDead && (NewValue <= 0.0))
	{
		UnitDied(Attacker);
	}
}

void APCUnit::UnitDied_Implementation(AActor* Killer)
{
	CancelCurrentAbility();

	IsDead = true;
	ActionableActorComponent->OnDied.Broadcast(Killer, this);
}

void APCUnit::TargetDied_Implementation(APCUnit* KillerActor, AActor* ActorKilled)
{
	if (ActorKilled == GetCurrentTarget())
	{
		CancelCurrentAbility();
	}
}

void APCUnit::ExecuteAction(FGameplayTag InputActionTag, FHitResult Hit)
{
	if (IsDead)
	{
		return;
	}

	if (!AbilitySystem)
	{
		return;
	}

	CancelCurrentAbility();

	if (Hit.GetActor())
	{
		UPCActionableActorComponent* ActionableComponent = Cast<UPCActionableActorComponent>(Hit.GetActor()->GetComponentByClass(UPCActionableActorComponent::StaticClass()));
		if (ActionableComponent && ActionableComponent->IsAlive() && AbilitySystem->ActivateAbility(InputActionTag, Hit))
		{
			OnTargetDiedDelegate = FScriptDelegate();
			OnTargetDiedDelegate.BindUFunction(this, "TargetDied");
			ActionableComponent->OnDied.Add(OnTargetDiedDelegate);
			return;
		}
	}
	
	if (Hit.bBlockingHit)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Hit.Location);
	}
}

void APCUnit::ExecuteAbility(FGameplayTag InputAbilityTag)
{

}

void APCUnit::CancelCurrentAbility()
{
	if (GetCurrentTarget())
	{
		UPCActionableActorComponent* ActionableComponent = Cast<UPCActionableActorComponent>(GetCurrentTarget()->GetComponentByClass(UPCActionableActorComponent::StaticClass()));
		if (ActionableComponent && ActionableComponent->IsAlive())
		{
			ActionableComponent->OnDied.Remove(OnTargetDiedDelegate);
		}
	}

	AbilitySystem->CancelCurrentAbility();
}

AActor* APCUnit::GetCurrentTarget()
{
	if (AbilitySystem)
	{
		TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(AbilitySystem->CurrentTargetData, 0);
		if (!TargetActors.IsEmpty())
		{
			return TargetActors[0];
		}
	}

	return nullptr;
}

void APCUnit::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APCUnit, Team);
	DOREPLIFETIME(APCUnit, Faction);
	DOREPLIFETIME(APCUnit, PlayerOwner);
	DOREPLIFETIME(APCUnit, IsAttacking);
	DOREPLIFETIME(APCUnit, IsDead);
}
