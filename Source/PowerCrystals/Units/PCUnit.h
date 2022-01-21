// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "../PCMeshMergeFunctionLibrary.h"
#include "../Abilities/PCAbilitySystemComponent.h"
#include "../Player/PCPlayerController.h"
#include "../PCActionableActorInterface.h"
#include "AbilitySystemInterface.h"
#include "PCUnit.generated.h"

UCLASS()
class POWERCRYSTALS_API APCUnit : public ACharacter, public IAbilitySystemInterface, public IPCActionableActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APCUnit();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// End IAbilitySystemInterface

	// Begin IActionableActorInterface
	virtual bool ExecuteAbility(FGameplayTag AbilityTag, FHitResult Hit) override;
	virtual bool AddTurnValue(float Value) override;
	virtual void TurnPressed(float Value) override;
	virtual void ActorSelected() override;
	virtual void ActorDeselected() override;
	virtual void SpawnPlayerUnit(TSubclassOf<class APCUnit> UnitBlueprint) override;
	virtual bool IsAlive() override;
	virtual bool IsPreview() override;
	virtual bool IsConstruction() override;
	virtual float GetConstructionTime() override;
	virtual class UPCAbilitySystemComponent* GetAbilitySystem() override;
	virtual FGameplayTag GetTeam() override;
	virtual FGameplayTag GetFaction() override;
	virtual class APCPlayerController* GetControllerOwner() override;
	virtual class UPCActionableActorComponent* GetActionableActorComponent() override;
	// End IActionableActorInterface

	UFUNCTION(BlueprintNativeEvent)
	void UnitSelected();

	UFUNCTION(BlueprintNativeEvent)
	void UnitDeselected();

	UFUNCTION(BlueprintNativeEvent)
	void UnitHealthChanged(float NewValue, AActor* Attacker);

	UFUNCTION(BlueprintNativeEvent)
	void UnitDied(AActor* Killer);

	UFUNCTION(BlueprintNativeEvent)
	void TargetDied(APCUnit* KillerActor, AActor* ActorKilled);

	UFUNCTION(BlueprintCallable)
	bool ExecuteUnitAbility(FGameplayTag InputAbilityTag, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	void CancelCurrentAbility();

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTarget();

	UFUNCTION(BlueprintCallable)
	void SetNewMovementSpeed(float NewMovementSpeed);

	UFUNCTION(BlueprintImplementableEvent)
	void InitUnit();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PCMesh)
	FPCSkeletalMeshMergeParams MeshMergeParameters;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTag Team;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTag Faction;

	UPROPERTY(BlueprintReadOnly, Replicated)
	APCPlayerController* PlayerOwner;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool IsAttacking;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool IsDead;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** The ability system component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AbilitySystem, meta = (AllowPrivateAccess = "true"))
	class UPCAbilitySystemComponent* AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ActionableActor, meta = (AllowPrivateAccess = "true"))
	class UPCActionableActorComponent* ActionableActorComponent;

private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	/** Delegate to get notified when the target unit or building dies. */
	FScriptDelegate OnTargetDiedDelegate;

	FScriptDelegate OnUnitHealthChangedDelegate;

};
