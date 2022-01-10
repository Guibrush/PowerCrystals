// Copyright Epic Games, Inc. All Rights Reserved.

#include "PCPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "PCPlayerCharacter.h"
#include "../Units/PCUnit.h"
#include "../Buildings/PCBuilding.h"
#include "../UI/PCHUD.h"
#include "../Components/PCActionableActorComponent.h"
#include "../Abilities/PCGameplayAbility.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"

APCPlayerController::APCPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	InputBlocked = false;
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
				UPCActionableActorComponent* ActionableComponent = Cast<UPCActionableActorComponent>(Hit.GetActor()->GetComponentByClass(UPCActionableActorComponent::StaticClass()));
				if (ActionableComponent && (ActionableComponent->GetTeam() == Team))
				{
					SelectedActors.AddUnique(Hit.GetActor());
					ActionableComponent->ActorSelected();
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
		UPCActionableActorComponent* ActionableComponent = Cast<UPCActionableActorComponent>(SelectedActor->GetComponentByClass(UPCActionableActorComponent::StaticClass()));
		if (ActionableComponent)
		{
			ActionableComponent->ActorDeselected();
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

		InputBlocked = false;
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

		InputBlocked = false;
	}
	else
	{
		if (SelectedActors.Num() > 0)
		{
			for (AActor* SelectedActor : SelectedActors)
			{
				UPCActionableActorComponent* ActionableComponent = Cast<UPCActionableActorComponent>(SelectedActor->GetComponentByClass(UPCActionableActorComponent::StaticClass()));
				if (ActionableComponent)
				{
					ActionableComponent->ExecuteAbility(ActionTag, Hit);
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
			UPCActionableActorComponent* ActionableComponent = Cast<UPCActionableActorComponent>(SelectedActor->GetComponentByClass(UPCActionableActorComponent::StaticClass()));
			if (ActionableComponent)
			{
				if (ActionableComponent->ExecuteAbility(AbilityTag, Hit))
				{
					InputBlocked = BlocksInput;
				}
			}
		}
	}
}

APCBuilding* APCPlayerController::SpawnBuilding(TSubclassOf<APCBuilding> BuildingBlueprint)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APCBuilding* NewBuilding = World->SpawnActorDeferred<APCBuilding>(BuildingBlueprint, GetPawn()->GetTransform(), this);
	if (NewBuilding)
	{
		NewBuilding->Team = Team;
		NewBuilding->Faction = Faction;
		NewBuilding->PlayerOwner = this;
		NewBuilding->HasPreview = true;
		NewBuilding->FinishSpawning(GetPawn()->GetTransform());
	}

	return NewBuilding;
}

void APCPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APCPlayerController, SelectedActors);
	DOREPLIFETIME(APCPlayerController, Team);
	DOREPLIFETIME(APCPlayerController, Faction);
	DOREPLIFETIME(APCPlayerController, InputBlocked);
}
