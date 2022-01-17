// Fill out your copyright notice in the Description page of Project Settings.


#include "PCUnit.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Buildings/PCBuilding.h"
#include "../Components/PCActionableActorComponent.h"
#include "../Abilities/PCAttributeSet.h"

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

	UPCAttributeSet* AttributeSet = Cast<UPCAttributeSet>(AbilitySystem->GetSpawnedAttributes()[0]);
	if (AttributeSet)
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed * AttributeSet->Movement;
	}

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

bool APCUnit::ExecuteAbility(FGameplayTag InputAbilityTag, FHitResult Hit)
{
	if (IsDead || !AbilitySystem)
	{
		return false;
	}

	CancelCurrentAbility();

	UPCActionableActorComponent* TargetActionableComponent;
	if (Hit.GetActor())
	{
		TargetActionableComponent = Cast<UPCActionableActorComponent>(Hit.GetActor()->GetComponentByClass(UPCActionableActorComponent::StaticClass()));
	}
	else
	{
		TargetActionableComponent = nullptr;
	}

	if (!TargetActionableComponent && Hit.bBlockingHit && (InputAbilityTag == PlayerOwner->ActionTag))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Hit.Location);
		return true;
	}

	bool AbilityActivated = AbilitySystem->ActivateAbility(InputAbilityTag, Hit, PlayerOwner);
	if (AbilityActivated && Hit.GetActor())
	{
		if (TargetActionableComponent && TargetActionableComponent->IsAlive())
		{
			OnTargetDiedDelegate = FScriptDelegate();
			OnTargetDiedDelegate.BindUFunction(this, "TargetDied");
			TargetActionableComponent->OnDied.Add(OnTargetDiedDelegate);
		}
	}

	return AbilityActivated;
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

	AbilitySystem->CancelCurrentAbility(PlayerOwner);
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

void APCUnit::SetNewMovementSpeed(float NewMovementSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewMovementSpeed;
}

UAbilitySystemComponent* APCUnit::GetAbilitySystemComponent() const
{
	return AbilitySystem;
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
