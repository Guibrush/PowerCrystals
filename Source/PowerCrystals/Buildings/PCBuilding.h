// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "../Player/PCPlayerController.h"
#include "PCBuilding.generated.h"

UCLASS()
class POWERCRYSTALS_API APCBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APCBuilding();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns ability system component. **/
	FORCEINLINE class UPCAbilitySystemComponent* GetAbilitySystem() { return AbilitySystem; }

	FORCEINLINE class USceneComponent* GetUnitsSpawnPoint() { return UnitsSpawnPoint; }

	FORCEINLINE class USceneComponent* GetUnitsRallyPoint() { return UnitsRallyPoint; }

	FORCEINLINE class UPCActionableActorComponent* GetActionableActorComponent() { return ActionableActorComponent; }

	UFUNCTION(BlueprintNativeEvent)
	void BuildingSelected();

	UFUNCTION(BlueprintNativeEvent)
	void BuildingDeselected();

	UFUNCTION(BlueprintNativeEvent)
	void BuildingHealthChanged(float NewValue, AActor* Attacker);

	UFUNCTION(BlueprintNativeEvent)
	void BuildingDestroyed(AActor* Killer);

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTag Team;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTag Faction;

	UPROPERTY(BlueprintReadOnly, Replicated)
	APCPlayerController* PlayerOwner;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool IsDestroyed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** The ability system component. */
	UPROPERTY(Category = Building, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPCAbilitySystemComponent* AbilitySystem;

	/** The main skeletal mesh associated with this building. */
	UPROPERTY(Category = Building, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh;

	/** The BoxComponent being used for collision. */
	UPROPERTY(Category = Building, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;

	UPROPERTY(Category = Building, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* UnitsSpawnPoint;

	UPROPERTY(Category = Building, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* UnitsRallyPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ActionableActor, meta = (AllowPrivateAccess = "true"))
	class UPCActionableActorComponent* ActionableActorComponent;

private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	FScriptDelegate OnBuildingHealthChangedDelegate;

};
