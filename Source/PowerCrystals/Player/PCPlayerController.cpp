// Copyright Epic Games, Inc. All Rights Reserved.

#include "PCPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "PCPlayerCharacter.h"
#include "../Units/PCUnit.h"
#include "../Buildings/PCBuilding.h"
#include "../UI/PCHUD.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

APCPlayerController::APCPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void APCPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void APCPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &APCPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APCPlayerController::MoveRight);

	InputComponent->BindAction("Selection", IE_Pressed, this, &APCPlayerController::SelectionPressed);
	InputComponent->BindAction("Selection", IE_Released, this, &APCPlayerController::SelectionReleased);

	InputComponent->BindAction("Action", IE_Pressed, this, &APCPlayerController::ActionPressed);
	InputComponent->BindAction("Action", IE_Released, this, &APCPlayerController::ActionReleased);

	InputComponent->BindAxis("Zoom", this, &APCPlayerController::Zoom);
}

void APCPlayerController::MoveForward(float Value)
{
	APCPlayerCharacter* const MyCharacter = Cast<APCPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		MyCharacter->MoveForward(Value);
	}
}

void APCPlayerController::MoveRight(float Value)
{
	APCPlayerCharacter* const MyCharacter = Cast<APCPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		MyCharacter->MoveRight(Value);
	}
}


void APCPlayerController::SelectionPressed()
{
	APCHUD* HUD = Cast<APCHUD>(GetHUD());
	if (HUD)
	{
		HUD->StartSelectionRectangle();
	}
}

void APCPlayerController::SelectionReleased()
{
	DeselectAllActors();

	TArray<APCUnit*> SelectedUnits;
	APCHUD* HUD = Cast<APCHUD>(GetHUD());
	if (HUD)
	{
		SelectedUnits = HUD->FinishSelectionRectangle();
	}

	if (SelectedUnits.Num() > 0)
	{
		for (APCUnit* SelectedUnit : SelectedUnits)
		{
			if (SelectedUnit->Team == Team)
			{
				SelectedActors.AddUnique(SelectedUnit);
				SelectedUnit->OnUnitSelected();
			}
		}
	}
	else
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.GetActor())
		{
			APCUnit* SelectedUnit = Cast<APCUnit>(Hit.GetActor());
			APCBuilding* SelectedBuilding = Cast<APCBuilding>(Hit.GetActor());

			if (SelectedUnit)
			{
				if (SelectedUnit->Team == Team)
				{
					SelectedActors.AddUnique(Hit.GetActor());
					SelectedUnit->OnUnitSelected();
				}
			}
			else if (SelectedBuilding)
			{
				if (SelectedBuilding->Team == Team)
				{
					SelectedActors.AddUnique(Hit.GetActor());
					SelectedBuilding->OnBuildingSelected();
				}
			}
		}
	}

	NotifyServerNewSelection(SelectedActors);
}

void APCPlayerController::DeselectAllActors()
{
	// Deselect all the actors previously selected
	for (AActor* SelectedActor : SelectedActors)
	{
		APCUnit* SelectedUnit = Cast<APCUnit>(SelectedActor);
		if (SelectedUnit)
		{
			SelectedUnit->OnUnitDeselected();
			continue;
		}

		APCBuilding* SelectedBuilding = Cast<APCBuilding>(SelectedActor);
		if (SelectedBuilding)
		{
			SelectedBuilding->OnBuildingDeselected();
			continue;
		}
	}

	SelectedActors.Empty();
}

void APCPlayerController::ActionPressed()
{

}

void APCPlayerController::ActionReleased()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	NotifyServerNewAction(Hit);
}

void APCPlayerController::Zoom(float Value)
{
	APCPlayerCharacter* const MyCharacter = Cast<APCPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		MyCharacter->Zoom(Value);
	}
}

void APCPlayerController::NotifyServerNewSelection_Implementation(const TArray<AActor*> &NewSelectedActors)
{
	SelectedActors = NewSelectedActors;
}

void APCPlayerController::NotifyServerNewAction_Implementation(FHitResult Hit)
{
	if (SelectedActors.Num() > 0)
	{
		for (AActor* SelectedActor : SelectedActors)
		{
			APCUnit* SelectedUnit = Cast<APCUnit>(SelectedActor);
			if (SelectedUnit)
			{
				SelectedUnit->ExecuteAction(InputActionTag, Hit);
			}
		}
	}
}

void APCPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APCPlayerController, SelectedActors);
	DOREPLIFETIME(APCPlayerController, Team);
	DOREPLIFETIME(APCPlayerController, Faction);
}
