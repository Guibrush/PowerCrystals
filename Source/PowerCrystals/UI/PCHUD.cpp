// Fill out your copyright notice in the Description page of Project Settings.


#include "PCHUD.h"
#include "../Player/PCPlayerController.h"
#include "EngineUtils.h"

void APCHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bDoSelectionRetancle)
	{
		ULocalPlayer* LocalPlayer = GetOwningPlayerController() ? Cast<ULocalPlayer>(GetOwningPlayerController()->Player) : NULL;
		if (LocalPlayer && LocalPlayer->ViewportClient)
		{
			FVector2D MousePosition;
			if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
			{
				FinalMousePos = MousePosition;
			}
		}

		if (FVector2D::Distance(InitialMousePos, FinalMousePos) > 10.0f)
		{
			FLinearColor RectColor = FLinearColor::Green;
			RectColor.A = 0.5f;
			DrawRect(RectColor, InitialMousePos.X, InitialMousePos.Y, FinalMousePos.X - InitialMousePos.X, FinalMousePos.Y - InitialMousePos.Y);

			GetUnitsInSelectionRectangle(InitialMousePos, FinalMousePos, SelectedUnits);
		}
	}
}

void APCHUD::StartSelectionRectangle()
{
	bDoSelectionRetancle = true;
	SelectedUnits.Empty();

	ULocalPlayer* LocalPlayer = GetOwningPlayerController() ? Cast<ULocalPlayer>(GetOwningPlayerController()->Player) : NULL;
	if (LocalPlayer && LocalPlayer->ViewportClient)
	{
		FVector2D MousePosition;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
		{
			InitialMousePos = MousePosition;
		}
	}
}

const TArray<APCUnit*> APCHUD::FinishSelectionRectangle()
{
	bDoSelectionRetancle = false;

	TArray<APCUnit*> TempSelectedUnits = SelectedUnits;
	SelectedUnits.Empty();

	return TempSelectedUnits;
}

const TArray<APCUnit*> APCHUD::GetTempSelectedUnits()
{
	return SelectedUnits;
}

void APCHUD::GetUnitsInSelectionRectangle(const FVector2D& FirstPoint, const FVector2D& SecondPoint, TArray<APCUnit*>& OutActors)
{
	APCPlayerController* PlayerController = Cast<APCPlayerController>(GetOwningPlayerController());
	if (!PlayerController)
	{
		return;
	}

	// Because this is a HUD function it is likely to get called each tick,
	// so make sure any previous contents of the out actor array have been cleared!
	OutActors.Reset();

	//Create Selection Rectangle from Points
	FBox2D SelectionRectangle(ForceInit);

	//This method ensures that an appropriate rectangle is generated, 
	//		no matter what the coordinates of first and second point actually are.
	SelectionRectangle += FirstPoint;
	SelectionRectangle += SecondPoint;

	for (TActorIterator<APCUnit> Itr(GetWorld()); Itr; ++Itr)
	{
		APCUnit* EachActor = *Itr;

		//Get Actor Bounds				//casting to base class, checked by template in the .h
		const FBox EachActorBounds = EachActor->GetComponentsBoundingBox(); /* All Components? */

		//Center
		const FVector BoxCenter = EachActorBounds.GetCenter();

		const FVector ProjectedWorldLocation = Project(BoxCenter);
		FVector2D ActorBox2D = FVector2D(ProjectedWorldLocation.X, ProjectedWorldLocation.Y);

		if (SelectionRectangle.IsInside(ActorBox2D))
		{
			OutActors.Add(EachActor);
		}
	}
}
