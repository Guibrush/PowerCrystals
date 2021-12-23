// Fill out your copyright notice in the Description page of Project Settings.

#include "PCUnitController.h"
#include "Navigation/CrowdFollowingComponent.h"

APCUnitController::APCUnitController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}
