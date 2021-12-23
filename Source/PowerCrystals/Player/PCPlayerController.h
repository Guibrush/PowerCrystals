// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "PCPlayerController.generated.h"

UCLASS()
class APCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APCPlayerController();

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<AActor*> SelectedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag InputActionTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> InputAbilitiesTags;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTag Team;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FGameplayTag Faction;

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

private:

	UFUNCTION(Server, Reliable)
	void NotifyServerNewSelection(const TArray<AActor*> &NewSelectedActors);

	UFUNCTION(Server, Reliable)
	void NotifyServerNewAction(FHitResult Hit);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};


