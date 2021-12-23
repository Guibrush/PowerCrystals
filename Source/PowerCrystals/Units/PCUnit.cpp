// Fill out your copyright notice in the Description page of Project Settings.


#include "PCUnit.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
APCUnit::APCUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystem = CreateDefaultSubobject<UPCAbilitySystemComponent>("AbilitySystem");
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

	InitUnit();
}

// Called every frame
void APCUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APCUnit::OnUnitSelected_Implementation()
{

}

void APCUnit::OnUnitDeselected_Implementation()
{

}

void APCUnit::OnUnitHealthChanged_Implementation(float NewValue, AActor* Attacker)
{
	OnHealthChanged.Broadcast(NewValue, Attacker);

	if (!IsDead && (NewValue <= 0.0))
	{
		OnUnitDied(Attacker);
	}
}

void APCUnit::OnUnitDied_Implementation(AActor* Killer)
{
	CancelCurrentAbility();

	IsDead = true;
	OnDied.Broadcast(Killer, this);
}

void APCUnit::OnTargetUnitDied_Implementation(APCUnit* KillerActor, AActor* ActorKilled)
{
	if (ActorKilled == GetCurrentTargetUnit())
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

	APCUnit* ActionableUnit = Cast<APCUnit>(Hit.GetActor());
	if (ActionableUnit && !ActionableUnit->IsDead)
	{
		if (AbilitySystem->ActivateAbility(InputActionTag, Hit))
		{
			OnTargetUnitDiedDelegate = FScriptDelegate();
			OnTargetUnitDiedDelegate.BindUFunction(this, "OnTargetUnitDied");
			ActionableUnit->OnDied.Add(OnTargetUnitDiedDelegate);
		}
	}
	else if (Hit.bBlockingHit)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Hit.Location);
	}
}

void APCUnit::ExecuteAbility(FGameplayTag InputAbilityTag)
{

}

void APCUnit::CancelCurrentAbility()
{
	APCUnit* TargetUnit = GetCurrentTargetUnit();
	if (TargetUnit)
	{
		TargetUnit->OnDied.Remove(OnTargetUnitDiedDelegate);
	}

	AbilitySystem->CancelCurrentAbility();
}

APCUnit* APCUnit::GetCurrentTargetUnit()
{
	if (AbilitySystem)
	{
		TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(AbilitySystem->CurrentTargetData, 0);
		if (!TargetActors.IsEmpty())
		{
			return Cast<APCUnit>(TargetActors[0]);
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
