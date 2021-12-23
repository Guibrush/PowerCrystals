// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "../Units/PCUnit.h"
#include "PCHUD.generated.h"

/**
 * 
 */
UCLASS()
class POWERCRYSTALS_API APCHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void DrawHUD() override;

	void StartSelectionRectangle();

	const TArray<APCUnit*> FinishSelectionRectangle();

	UFUNCTION(BlueprintCallable)
	const TArray<APCUnit*> GetTempSelectedUnits();

private:

	void GetUnitsInSelectionRectangle(const FVector2D& FirstPoint, const FVector2D& SecondPoint, TArray<APCUnit*>& OutActors);

	bool bDoSelectionRetancle = false;

	FVector2D InitialMousePos;
	FVector2D FinalMousePos;

	TArray<APCUnit*> SelectedUnits;
	
};
