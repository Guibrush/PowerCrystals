// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTagContainer.h"
#include "../Units/PCUnit.h"
#include "../Buildings/PCBuilding.h"
#include "PCGameMode.generated.h"

UCLASS(minimalapi)
class APCGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	APCGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FGameplayTag> PlayerTeams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EnemyTeam;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag PlayersFaction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EnemyFaction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APCBuilding> InitialPlayerBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<APCUnit>> InitialPlayerUnits;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<APCUnit>> InitialEnemyUnits;

	//~ Begin AGameModeBase Interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;
	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual void StartPlay() override;
	//~ End AGameModeBase Interface

private:

	void AssignPlayerTeam(AController* Controller);

	void SpawnPlayerBaseAndUnits(AController* Controller);

	APCUnit* SpawnEnemyUnit(FTransform StartTransform, TSubclassOf<APCUnit> UnitBlueprint);

	int32 CurrentTeamIndex;
};



