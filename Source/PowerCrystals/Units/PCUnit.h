// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../PCMeshMergeFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "../Abilities/PCAbilitySystemComponent.h"
#include "../Player/PCPlayerController.h"
#include "PCUnit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUnitHealthChange, float, NewValue, AActor*, Attacker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUnitDied, AActor*, KillerActor, AActor*, ActorKilled);

UCLASS()
class POWERCRYSTALS_API APCUnit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APCUnit();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UPCAbilitySystemComponent* GetAbilitySystem() { return AbilitySystem; }

	UFUNCTION(BlueprintNativeEvent)
	void OnUnitSelected();

	UFUNCTION(BlueprintNativeEvent)
	void OnUnitDeselected();

	UFUNCTION(BlueprintNativeEvent)
	void OnUnitHealthChanged(float NewValue, AActor* Attacker);

	UFUNCTION(BlueprintNativeEvent)
	void OnUnitDied(AActor* Killer);

	UFUNCTION(BlueprintNativeEvent)
	void OnTargetUnitDied(APCUnit* KillerActor, AActor* ActorKilled);

	UFUNCTION(BlueprintCallable)
	void ExecuteAction(FGameplayTag InputActionTag, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	void ExecuteAbility(FGameplayTag InputAbilityTag);

	UFUNCTION(BlueprintCallable)
	void CancelCurrentAbility();

	UFUNCTION(BlueprintCallable)
	APCUnit* GetCurrentTargetUnit();

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

	UPROPERTY(BlueprintAssignable)
	FUnitHealthChange OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FUnitDied OnDied;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPCAbilitySystemComponent* AbilitySystem;

private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	/** Delegate to get notified when the target unit dies. */
	FScriptDelegate OnTargetUnitDiedDelegate;

};
