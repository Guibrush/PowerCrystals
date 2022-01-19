// Copyright Epic Games, Inc. All Rights Reserved.

#include "PCPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "PCPlayerCharacter.h"
#include "../Units/PCUnit.h"
#include "../Buildings/PCBuilding.h"
#include "../UI/PCHUD.h"
#include "../Abilities/PCGameplayAbility.h"
#include "../PCActionableActorInterface.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"

APCPlayerController::APCPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	InputBlocked = false;

	TurnRight = false;
	TurnLeft = false;
}

void APCPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CheckTurnValues();
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

	InputComponent->BindAction("TurnRight", IE_Pressed, this, &APCPlayerController::TurnRightPressed);
	InputComponent->BindAction("TurnRight", IE_Released, this, &APCPlayerController::TurnRightReleased);

	InputComponent->BindAction("TurnLeft", IE_Pressed, this, &APCPlayerController::TurnLeftPressed);
	InputComponent->BindAction("TurnLeft", IE_Released, this, &APCPlayerController::TurnLeftReleased);
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
	if (InputBlocked)
	{
		return;
	}

	APCHUD* HUD = Cast<APCHUD>(GetHUD());
	if (HUD)
	{
		HUD->StartSelectionRectangle();
	}
}

void APCPlayerController::SelectionReleased()
{
	if (!InputBlocked)
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
					SelectedUnit->UnitSelected();
				}
			}
		}
		else
		{
			FHitResult Hit;
			GetHitResultUnderCursor(ECC_Visibility, false, Hit);

			if (Hit.GetActor())
			{
				IPCActionableActorInterface* ActionableActor = Cast<IPCActionableActorInterface>(Hit.GetActor());
				if (ActionableActor)
				{
					SelectedActors.AddUnique(Hit.GetActor());
					ActionableActor->ActorSelected();
				}
			}
		}

		OnNewSelectedActors.Broadcast();
	}

	NotifyServerNewSelection(SelectedActors);
}

void APCPlayerController::DeselectAllActors()
{
	// Deselect all the actors previously selected
	for (AActor* SelectedActor : SelectedActors)
	{
		IPCActionableActorInterface* ActionableActor = Cast<IPCActionableActorInterface>(SelectedActor);
		if (ActionableActor)
		{
			ActionableActor->ActorDeselected();
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

void APCPlayerController::TurnRightPressed()
{
	TurnRight = true;
}

void APCPlayerController::TurnRightReleased()
{
	TurnRight = false;
}

void APCPlayerController::TurnLeftPressed()
{
	TurnLeft = true;
}

void APCPlayerController::TurnLeftReleased()
{
	TurnLeft = false;
}

void APCPlayerController::CheckTurnValues()
{
	float TurnValue = 0.0f;

	if (TurnRight)
	{
		TurnValue += -1.0f;
	}

	if (TurnLeft)
	{
		TurnValue += 1.0f;
	}

	if (TurnValue != 0.0f)
	{
		APCPlayerCharacter* const MyCharacter = Cast<APCPlayerCharacter>(GetPawn());
		if (MyCharacter)
		{
			MyCharacter->AddCameraRotation(TurnValue);
		}
	}
}

void APCPlayerController::ExecuteAbility(FGameplayTag AbilityTag, bool BlocksInput)
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	NotifyServerNewAbility(AbilityTag, Hit, BlocksInput);
}

void APCPlayerController::NotifyServerNewSelection_Implementation(const TArray<AActor*> &NewSelectedActors)
{
	if (InputBlocked)
	{
		if (SelectedActors.Num() > 0)
		{
			for (AActor* SelectedActor : SelectedActors)
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SelectedActor, SelectionTag, FGameplayEventData());
			}
		}
		else
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), SelectionTag, FGameplayEventData());
		}
	}
	else
	{
		SelectedActors = NewSelectedActors;
	}
}

void APCPlayerController::NotifyServerNewAction_Implementation(FHitResult Hit)
{
	if (InputBlocked)
	{
		if (SelectedActors.Num() > 0)
		{
			for (AActor* SelectedActor : SelectedActors)
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SelectedActor, ActionTag, FGameplayEventData());
			}
		}
		else
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), ActionTag, FGameplayEventData());
		}
	}
	else
	{
		if (SelectedActors.Num() > 0)
		{
			for (AActor* SelectedActor : SelectedActors)
			{
				IPCActionableActorInterface* ActionableActor = Cast<IPCActionableActorInterface>(SelectedActor);
				if (ActionableActor)
				{
					ActionableActor->ExecuteAbility(ActionTag, Hit);
				}
			}
		}
	}
}

void APCPlayerController::NotifyServerNewAbility_Implementation(FGameplayTag AbilityTag, FHitResult Hit, bool BlocksInput)
{
	if (InputBlocked)
	{
		return;
	}

	if (AbilityTag.MatchesAny(FGameplayTagContainer(PlayerAbilityTag)))
	{
		APCPlayerCharacter* PlayerCharacter = Cast<APCPlayerCharacter>(GetPawn());
		if (PlayerCharacter)
		{
			if (PlayerCharacter->ExecuteAbility(AbilityTag, Hit))
			{
				InputBlocked = BlocksInput;
			}
		}
	}
	else if (SelectedActors.Num() > 0)
	{
		for (AActor* SelectedActor : SelectedActors)
		{
			IPCActionableActorInterface* ActionableActor = Cast<IPCActionableActorInterface>(SelectedActor);
			if (ActionableActor)
			{
				if (ActionableActor->ExecuteAbility(AbilityTag, Hit))
				{
					InputBlocked = BlocksInput;
				}
			}
		}
	}
}

APCBuilding* APCPlayerController::SpawnBuilding(TSubclassOf<APCBuilding> BuildingBlueprint, FTransform StartTransform, bool WithPreview)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APCBuilding* NewBuilding = World->SpawnActorDeferred<APCBuilding>(BuildingBlueprint, StartTransform, this);
	if (NewBuilding)
	{
		NewBuilding->Team = Team;
		NewBuilding->Faction = Faction;
		NewBuilding->PlayerOwner = this;
		NewBuilding->HasPreview = WithPreview;
		NewBuilding->FinishSpawning(GetPawn()->GetTransform());
	}

	return NewBuilding;
}

void APCPlayerController::UnblockInput()
{
	InputBlocked = false;
}

void APCPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APCPlayerController, SelectedActors);
	DOREPLIFETIME(APCPlayerController, Team);
	DOREPLIFETIME(APCPlayerController, Faction);
	DOREPLIFETIME(APCPlayerController, InputBlocked);
}
