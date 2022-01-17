// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "PCBuilding.generated.h"

UCLASS()
class POWERCRYSTALS_API APCBuilding : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APCBuilding();

	virtual void Tick(float DeltaTime) override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	/** Returns ability system component. **/
	FORCEINLINE class UPCAbilitySystemComponent* GetAbilitySystem() { return AbilitySystem; }

	FORCEINLINE class USceneComponent* GetUnitsSpawnPoint() { return UnitsSpawnPoint; }

	FORCEINLINE class USceneComponent* GetUnitsRallyPoint() { return UnitsRallyPoint; }

	FORCEINLINE class UPCActionableActorComponent* GetActionableActorComponent() { return ActionableActorComponent; }

	FORCEINLINE class UPCTaskSystemComponent* GetTaskSystem() { return TaskSystem; }

	// Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// End IAbilitySystemInterface

	void SpawnUnitsQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	UFUNCTION(BlueprintNativeEvent)
	void BuildingSelected();

	UFUNCTION(BlueprintNativeEvent)
	void BuildingDeselected();

	UFUNCTION(BlueprintNativeEvent)
	void BuildingHealthChanged(float NewValue, AActor* Attacker);

	UFUNCTION(BlueprintNativeEvent)
	void BuildingDestroyed(AActor* Killer);

	UFUNCTION(BlueprintImplementableEvent)
	void BPInitPreviewMode();

	UFUNCTION(BlueprintImplementableEvent)
	void BPInitConstructionMode();

	UFUNCTION(BlueprintImplementableEvent)
	void BPBuildingConstructed();

	UFUNCTION(BlueprintImplementableEvent)
	void BPNewBuildingMode(bool NewIsInConstruction, bool NewIsInPreview);

	UFUNCTION(BlueprintCallable)
	void InitPreviewMode();

	UFUNCTION(BlueprintCallable)
	bool InitConstructionMode();

	UFUNCTION(BlueprintCallable)
	void BuildingConstructed();

	UFUNCTION(BlueprintCallable)
	bool ExecuteAbility(FGameplayTag InputAbilityTag, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	bool SpawnPlayerUnits(TArray<TSubclassOf<APCUnit>> UnitBlueprints);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastNewBuildingModeAndCollision(bool NewIsInConstruction, bool NewIsInPreview, ECollisionEnabled::Type NewCollisionMode);

	UFUNCTION(Server, Reliable)
	void ServerNewMouseProjectedPoint(FVector NewPoint);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEnvQuery* SpawnUnitsLocationQuery;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTag Team;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTag Faction;

	UPROPERTY(BlueprintReadOnly, Replicated)
	class APCPlayerController* PlayerOwner;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool IsDestroyed;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool IsInPreview;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool IsInConstruction;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool HasPreview;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool ValidPosition;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Building, meta = (AllowPrivateAccess = "true"))
	class UPCTaskSystemComponent* TaskSystem;

private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	FScriptDelegate OnBuildingHealthChangedDelegate;

	UPROPERTY(Replicated)
	FVector MouseProjectedPoint;

	TArray<TSubclassOf<APCUnit>> UnitToSpawnBlueprints;

};
