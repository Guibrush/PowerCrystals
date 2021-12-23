// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PCUnitController.generated.h"

/**
 * 
 */
UCLASS()
class POWERCRYSTALS_API APCUnitController : public AAIController
{
	GENERATED_BODY()

public:

	APCUnitController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
