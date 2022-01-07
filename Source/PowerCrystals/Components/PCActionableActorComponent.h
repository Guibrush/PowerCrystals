// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PCActionableActorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewValue, AActor*, Attacker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDied, AActor*, KillerActor, AActor*, ActorKilled);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POWERCRYSTALS_API UPCActionableActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPCActionableActorComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitComponent(bool NewIsUnit, bool NewIsBuilding);

	void HealthChanged(float NewHealth, AActor* Attacker);

	void ActorSelected();

	void ActorDeselected();

	UFUNCTION(NetMulticast, Reliable)
	void HealthChangedMulticast(float NewHealth, AActor* Attacker);

	UFUNCTION(BlueprintCallable)
	bool ExecuteAbility(FGameplayTag AbilityTag, FHitResult Hit);

	UFUNCTION(BlueprintPure)
	bool IsAlive();

	UFUNCTION(BlueprintPure)
	class UPCAbilitySystemComponent* GetAbilitySystem();

	UFUNCTION(BlueprintPure)
	FGameplayTag GetTeam();

	UFUNCTION(BlueprintPure)
	FGameplayTag GetFaction();

	UPROPERTY(BlueprintReadOnly)
	bool IsUnit;

	UPROPERTY(BlueprintReadOnly)
	bool IsBuilding;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnDied OnDied;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
};
