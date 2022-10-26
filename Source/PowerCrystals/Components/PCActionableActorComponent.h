// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PCActionableActorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewValue, AActor*, Attacker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDied, AActor*, KillerActor, AActor*, ActorKilled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNewBuildingMode, AActor*, Building, bool, IsInConstruction, bool, IsInPreview);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POWERCRYSTALS_API UPCActionableActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPCActionableActorComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ActorSelected();

	void ActorDeselected();

	void HealthChanged(float NewHealth, AActor* Attacker);

	void ActorDied(AActor* Killer);

	void NewBuildingMode(bool NewIsInConstruction, bool NewIsInPreview);

	UFUNCTION(NetMulticast, Reliable)
	void HealthChangedMulticast(float NewHealth, AActor* Attacker);

	UFUNCTION(NetMulticast, Reliable)
	void ActorDiedMulticast(AActor* Killer);

	UFUNCTION(NetMulticast, Reliable)
	void NewBuildingModeMulticast(bool NewIsInConstruction, bool NewIsInPreview);

	UFUNCTION(BlueprintCallable)
	bool ExecuteAbility(FGameplayTag AbilityTag, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	void SpawnPlayerUnit(TSubclassOf<class APCUnit> UnitBlueprint);

	UFUNCTION(BlueprintPure)
	bool IsAlive();

	UFUNCTION(BlueprintPure)
	bool IsInPreview();

	UFUNCTION(BlueprintPure)
	bool IsInConstruction();

	UFUNCTION(BlueprintPure)
	float GetConstructionTime();

	UFUNCTION(BlueprintPure)
	class UPCAbilitySystemComponent* GetAbilitySystem();

	UFUNCTION(BlueprintPure)
	FGameplayTag GetTeam();

	UFUNCTION(BlueprintPure)
	FGameplayTag GetFaction();

	UFUNCTION(BlueprintPure)
	class APCPlayerController* GetControllerOwner();

	UFUNCTION(BlueprintPure)
	class UPCTechTreeSystemComponent* GetOwningPlayerTechTreeSystem();

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnDied OnDied;

	UPROPERTY(BlueprintAssignable)
	FOnNewBuildingMode OnNewBuildingMode;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	class IPCActionableActorInterface* ActionableActor;
};
