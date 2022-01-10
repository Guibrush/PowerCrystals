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
		FVector LocationDirection = FVector::XAxisVector.RotateAngleAxis(FMath::FRandRange(-180, 180), FVector::ZAxisVector);
		FVector NewLocation = StartLocation + (LocationDirection * index * 250.0f);

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
	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	APCPlayerController* PCController = Cast<APCPlayerController>(Controller);
	if (PCController)
	{
		FVector StartLocation;
		ECollisionChannel Channel = ECC_WorldStatic;
		FHitResult OutHit;
		FVector StartPos = PCController->StartSpot->GetActorLocation() + (FVector::UpVector * 1000.0f);
		FVector EndPos = PCController->StartSpot->GetActorLocation() + (FVector::DownVector * 1000.0f);
		bool TraceHit = World->LineTraceSingleByChannel(OutHit, StartPos, EndPos, Channel);
		if (TraceHit)
		{
			StartLocation = OutHit.ImpactPoint;
		}
		else
		{
			StartLocation = PCController->StartSpot->GetActorLocation();
		}

		FRotator StartRotation = PCController->StartSpot->GetActorRotation();
		APCBuilding* InitialBuilding = nullptr;
		if (InitialPlayerBuilding)
		{
			FTransform StartTransform = FTransform(StartRotation, StartLocation);
			InitialBuilding = SpawnPlayerBuilding(StartTransform, InitialPlayerBuilding, PCController);
		}

		if (InitialBuilding)
		{
			FVector UnitsLocation = InitialBuilding->GetUnitsSpawnPoint()->GetComponentLocation();
			FRotator UnitsRotation = InitialBuilding->GetUnitsSpawnPoint()->GetComponentRotation();
			FVector SpawnDirection = (InitialBuilding->GetActorLocation() - UnitsLocation) * -1;
			SpawnDirection.Normalize();
			int32 index = 0;
			for (TSubclassOf<APCUnit> UnitBlueprint : InitialPlayerUnits)
			{
				FVector LocationDirection = SpawnDirection.RotateAngleAxis(FMath::FRandRange(-90, 90), FVector::ZAxisVector);
				FVector NewLocation = UnitsLocation + (LocationDirection * index * 200.0f);

				FTransform StartTransform = FTransform(UnitsRotation, NewLocation);
				SpawnPlayerUnit(StartTransform, UnitBlueprint, PCController);

				index++;
			}
		}
	}
}

APCUnit* APCGameMode::SpawnPlayerUnit(FTransform StartTransform, TSubclassOf<APCUnit> UnitBlueprint, APCPlayerController* PCController)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APCUnit* NewUnit = World->SpawnActorDeferred<APCUnit>(UnitBlueprint, StartTransform, PCController, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (NewUnit)
	{
		NewUnit->Team = PCController->Team;
		NewUnit->Faction = PCController->Faction;
		NewUnit->PlayerOwner = PCController;
		NewUnit->FinishSpawning(StartTransform);
	}

	return NewUnit;
}

APCBuilding* APCGameMode::SpawnPlayerBuilding(FTransform StartTransform, TSubclassOf<APCBuilding> BuildingBlueprint, APCPlayerController* PCController)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APCBuilding* NewBuilding = World->SpawnActorDeferred<APCBuilding>(BuildingBlueprint, StartTransform, PCController);
	if (NewBuilding)
	{
		NewBuilding->Team = PCController->Team;
		NewBuilding->Faction = PCController->Faction;
		NewBuilding->PlayerOwner = PCController;
		NewBuilding->HasPreview = false;
		NewBuilding->FinishSpawning(StartTransform);
	}

	return NewBuilding;
}

APCUnit* APCGameMode::SpawnEnemyUnit(FTransform StartTransform, TSubclassOf<APCUnit> UnitBlueprint)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APCUnit* NewUnit = World->SpawnActorDeferred<APCUnit>(UnitBlueprint, StartTransform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (NewUnit)
	{
		NewUnit->Team = EnemyTeam;
		NewUnit->Faction = EnemyFaction;
		NewUnit->PlayerOwner = nullptr;
		NewUnit->FinishSpawning(StartTransform);
	}

	return NewUnit;
}
