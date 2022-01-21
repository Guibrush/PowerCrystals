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

	if (ActionableActorComponent)
	{
		ActionableActorComponent->ActorDied(Killer);
	}
}

void APCUnit::TargetDied_Implementation(APCUnit* KillerActor, AActor* ActorKilled)
{
	if (ActorKilled == GetCurrentTarget())
	{
		CancelCurrentAbility();
	}
}

bool APCUnit::ExecuteUnitAbility(FGameplayTag InputAbilityTag, FHitResult Hit)
{
	if (IsDead || !AbilitySystem)
	{
		return false;
	}

	CancelCurrentAbility();

	IPCActionableActorInterface* ActionableActor;
	if (Hit.GetActor())
	{
		ActionableActor = Cast<IPCActionableActorInterface>(Hit.GetActor());
	}
	else
	{
		ActionableActor = nullptr;
	}

	if (!ActionableActor && Hit.bBlockingHit && (InputAbilityTag == PlayerOwner->ActionTag))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Hit.Location);
		return true;
	}

	bool AbilityActivated = AbilitySystem->ActivateAbility(InputAbilityTag, Hit, PlayerOwner);
	if (AbilityActivated && Hit.GetActor())
	{
		if (ActionableActor && ActionableActor->IsAlive())
		{
			UPCActionableActorComponent* TargetActionableComponent = Cast<UPCActionableActorComponent>(Hit.GetActor()->GetComponentByClass(UPCActionableActorComponent::StaticClass()));
			if (TargetActionableComponent)
			{
				OnTargetDiedDelegate = FScriptDelegate();
				OnTargetDiedDelegate.BindUFunction(this, "TargetDied");
				TargetActionableComponent->OnDied.Add(OnTargetDiedDelegate);
			}
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

bool APCUnit::ExecuteAbility(FGameplayTag AbilityTag, FHitResult Hit)
{
	return ExecuteUnitAbility(AbilityTag, Hit);
}

bool APCUnit::AddTurnValue(float Value)
{
	return false;
}

void APCUnit::TurnPressed(float Value)
{

}

void APCUnit::ActorSelected()
{
	UnitSelected();
}

void APCUnit::ActorDeselected()
{
	UnitDeselected();
}

void APCUnit::SpawnPlayerUnit(TSubclassOf<class APCUnit> UnitBlueprint)
{

}

bool APCUnit::IsAlive()
{
	return !IsDead;
}

bool APCUnit::IsPreview()
{
	return false;
}

bool APCUnit::IsConstruction()
{
	return false;
}

float APCUnit::GetConstructionTime()
{
	return 0.0f;
}

UPCAbilitySystemComponent* APCUnit::GetAbilitySystem()
{
	return AbilitySystem;
}

FGameplayTag APCUnit::GetTeam()
{
	return Team;
}

FGameplayTag APCUnit::GetFaction()
{
	return Faction;
}

APCPlayerController* APCUnit::GetControllerOwner()
{
	return PlayerOwner;
}

UPCActionableActorComponent* APCUnit::GetActionableActorComponent()
{
	return ActionableActorComponent;
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
