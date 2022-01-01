// Fill out your copyright notice in the Description page of Project Settings.


#include "PCCrowdFollowingComponent.h"

void UPCCrowdFollowingComponent::DeactivateCrowdComponent()
{
	SetCrowdSimulationState(ECrowdSimulationState::Disabled);
}
