// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "GameplayTagContainer.h"
#include "PCPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class POWERCRYSTALS_API APCPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Team;
	
};
