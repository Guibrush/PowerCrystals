// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../PCMeshMergeFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "../Abilities/PCAbilitySystemComponent.h"
#include "../Player/PCPlayerController.h"
#include "PCUnit.generated.h"

UCLASS()
class POWERCRYSTALS_API APCUnit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APCUnit();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns ability system component. **/
	FORCEINLINE class UPCAbilitySystemComponent* GetAbilitySystem() { return AbilitySystem; }

	FORCEINLINE class UPCActionableActorComponent* GetActionableActorComponent() { return ActionableActorComponent; }

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
	void ExecuteAction(FGameplayTag InputActionTag, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	void ExecuteAbility(FGameplayTag InputAbilityTag);

	UFUNCTION(BlueprintCallable)
	void CancelCurrentAbility();

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTarget();

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
