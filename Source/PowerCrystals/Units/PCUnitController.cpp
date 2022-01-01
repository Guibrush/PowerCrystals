// Fill out your copyright notice in the Description page of Project Settings.

#include "PCUnitController.h"
#include "../Components/PCCrowdFollowingComponent.h"

APCUnitController::APCUnitController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPCCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}
