// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "PCCrowdFollowingComponent.generated.h"

/**
 * 
 */
UCLASS()
class POWERCRYSTALS_API UPCCrowdFollowingComponent : public UCrowdFollowingComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void DeactivateCrowdComponent();
	
};
