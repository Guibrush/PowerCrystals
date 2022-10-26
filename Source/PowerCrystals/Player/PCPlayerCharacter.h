// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "PCPlayerCharacter.generated.h"

UCLASS()
class POWERCRYSTALS_API APCPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APCPlayerCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	/** Returns ability system component. **/
	FORCEINLINE class UPCAbilitySystemComponent* GetAbilitySystem() { return AbilitySystem; }
	/** Returns tech tree system component. **/
	FORCEINLINE class UPCTechTreeSystemComponent* GetTechTreeSystem() { return TechTreeSystem; }

	/** Movement functions. */
	void MoveForward(float Value);
	void MoveRight(float Value);

	void Zoom(float Value);

	void AddCameraRotation(float Value);

	UFUNCTION()
	void OnUnitSpawned(AActor* BuildingSpawner, AActor* NewUnit);

	UFUNCTION(BlueprintCallable)
	bool ExecuteAbility(FGameplayTag AbilityTag, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	void CancelCurrentAbility();

	UFUNCTION(BlueprintCallable)
	class APCBuilding* SpawnBuilding(TSubclassOf<class APCBuilding> BuildingBlueprint, FTransform StartTransform, bool WithPreview = true);

	UFUNCTION(BlueprintCallable)
	void AddNewPlayerUnit(class APCUnit* NewUnit);

	UFUNCTION(BlueprintCallable)
	void RemovePlayerUnit(class APCUnit* UnitToRemove);

	UFUNCTION(BlueprintCallable)
	void AddNewPlayerBuilding(class APCBuilding* NewBuilding);

	UFUNCTION(BlueprintCallable)
	void RemovePlayerBuilding(class APCBuilding* BuildingToRemove);

	UFUNCTION(BlueprintPure)
	TArray<class APCUnit*> GetPlayerUnits() const;

	UFUNCTION(BlueprintPure)
	TArray<class APCBuilding*> GetPlayerBuildings() const;

	UFUNCTION(BlueprintPure)
	TArray<AActor*> GetPlayerActionableActors() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraZoomSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraRotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxCameraRotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraRotationDecelerateSpeed;

protected:

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<class APCUnit*> PlayerUnits;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<class APCBuilding*> PlayerBuildings;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	/** The ability system component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AbilitySystem, meta = (AllowPrivateAccess = "true"))
	class UPCAbilitySystemComponent* AbilitySystem;

	/** The tech tree system component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TechTree, meta = (AllowPrivateAccess = "true"))
	class UPCTechTreeSystemComponent* TechTreeSystem;

	float CurrentCameraRotationSpeed;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

};
