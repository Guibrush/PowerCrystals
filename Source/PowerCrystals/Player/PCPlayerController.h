// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "PCPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNewSelectedActors);

UCLASS()
class APCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APCPlayerController();

	UFUNCTION(BlueprintCallable)
	void ExecuteAbility(FGameplayTag AbilityTag, bool BlocksInput);

	UFUNCTION(BlueprintCallable)
	class APCBuilding* SpawnBuilding(TSubclassOf<class APCBuilding> BuildingBlueprint, FTransform StartTransform, bool WithPreview = true);

	UFUNCTION(BlueprintCallable)
	void UnblockInput();

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<AActor*> SelectedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag PlayerAbilityTag;

	/** Tag associated to the left mouse click. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SelectionTag;

	/** Tag associated to the right mouse click. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ActionTag;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTag Team;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTag Faction;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool InputBlocked;

	UPROPERTY(BlueprintAssignable)
	FNewSelectedActors OnNewSelectedActors;

protected:

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Input handlers for moving the character. */
	void MoveForward(float Value);
	void MoveRight(float Value);

	void SelectionPressed();
	void SelectionReleased();

	void ActionPressed();
	void ActionReleased();

	void Zoom(float Value);

	void TurnRightPressed();
	void TurnRightReleased();

	void TurnLeftPressed();
	void TurnLeftReleased();

private:

	UFUNCTION(Server, Reliable)
	void NotifyServerNewSelection(const TArray<AActor*> &NewSelectedActors);

	UFUNCTION(Server, Reliable)
	void NotifyServerNewAction(FHitResult Hit);

	UFUNCTION(Server, Reliable)
	void NotifyServerNewAbility(FGameplayTag AbilityTag, FHitResult Hit, bool BlocksInput);

	void DeselectAllActors();

	void CheckTurnValues();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	bool TurnRight;
	bool TurnLeft;
};


