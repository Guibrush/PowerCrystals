// Copyright Epic Games, Inc. All Rights Reserved.

#include "PCGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "../Player/PCPlayerController.h"
#include "../Game/PCPlayerStart.h"
#include "Kismet/GameplayStatics.h"

APCGameMode::APCGameMode()
{

}

void APCGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	CurrentTeamIndex = 0;
}

FString APCGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	AssignPlayerTeam(NewPlayerController);

	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void APCGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	AssignPlayerTeam(NewController);

	Super::InitSeamlessTravelPlayer(NewController);
}

void APCGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	SpawnPlayerBaseAndUnits(NewPlayer);
}

void APCGameMode::StartPlay()
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	APCPlayerStart* EnemyPlayerStart = nullptr;
	TArray<AActor*> FoundPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(World, APCPlayerStart::StaticClass(), FoundPlayerStarts);
	for (AActor* PlayerStart : FoundPlayerStarts)
	{
		APCPlayerStart* PCPlayerStart = Cast<APCPlayerStart>(PlayerStart);
		if (PCPlayerStart && PCPlayerStart->Team == EnemyTeam)
		{
			EnemyPlayerStart = PCPlayerStart;
			break;
		}
	}

	if (!EnemyPlayerStart)
	{
		return;
	}

	FVector StartLocation = EnemyPlayerStart->GetActorLocation();
	FRotator StartRotation = EnemyPlayerStart->GetActorRotation();
	int32 index = 0;
	for (TSubclassOf<APCUnit> EnemyBlueprint : InitialEnemyUnits)
	{
		FVector LocationDirection = FVector::XAxisVector.RotateAngleAxis((36 * (index % 10)), FVector::ZAxisVector);
		FVector NewLocation;
		if (index == 0)
		{
			NewLocation = StartLocation;
		}
		else
		{
			NewLocation = StartLocation + (LocationDirection * (FMath::TruncToFloat(index / 10) + 1) * 600);
		}

		FTransform StartTransform = FTransform(StartRotation, NewLocation);
		SpawnEnemyUnit(StartTransform, EnemyBlueprint);

		index++;
	}

	Super::StartPlay();
}

void APCGameMode::AssignPlayerTeam(AController* Controller)
{
	APCPlayerController* PCController = Cast<APCPlayerController>(Controller);
	if (PCController)
	{
		PCController->Team = PlayerTeams[CurrentTeamIndex];
		PCController->Faction = PlayersFaction;
		CurrentTeamIndex++;
	}
}

void APCGameMode::SpawnPlayerBaseAndUnits(AController* Controller)
{
	APCPlayerController* PCController = Cast<APCPlayerController>(Controller);
	if (PCController)
	{
		FVector StartLocation = PCController->StartSpot->GetActorLocation();
		FRotator StartRotation = PCController->StartSpot->GetActorRotation();
		int32 index = 0;
		for (TSubclassOf<APCUnit> UnitBlueprint : InitialPlayerUnits)
		{
			FVector LocationDirection = FVector::XAxisVector.RotateAngleAxis((36 * (index % 10)), FVector::ZAxisVector);
			FVector NewLocation;
			if (index == 0)
			{
				NewLocation = StartLocation;
			}
			else
			{
				NewLocation = StartLocation + (LocationDirection * (FMath::TruncToFloat(index / 10) + 1) * 250);
			}

			FTransform StartTransform = FTransform(StartRotation, NewLocation);
			SpawnPlayerUnit(StartTransform, UnitBlueprint, PCController);

			index++;
		}
	}
}

void APCGameMode::SpawnPlayerUnit(FTransform StartTransform, TSubclassOf<APCUnit> UnitBlueprint, APCPlayerController* PCController)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	APCUnit* NewUnit = World->SpawnActorDeferred<APCUnit>(UnitBlueprint, StartTransform, GetOwner());
	if (NewUnit)
	{
		NewUnit->Team = PCController->Team;
		NewUnit->Faction = PCController->Faction;
		NewUnit->PlayerOwner = PCController;
		NewUnit->FinishSpawning(StartTransform);
	}
}

void APCGameMode::SpawnEnemyUnit(FTransform StartTransform, TSubclassOf<APCUnit> UnitBlueprint)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	APCUnit* NewUnit = World->SpawnActorDeferred<APCUnit>(UnitBlueprint, StartTransform, GetOwner());
	if (NewUnit)
	{
		NewUnit->Team = EnemyTeam;
		NewUnit->Faction = EnemyFaction;
		NewUnit->PlayerOwner = nullptr;
		NewUnit->FinishSpawning(StartTransform);
	}
}
