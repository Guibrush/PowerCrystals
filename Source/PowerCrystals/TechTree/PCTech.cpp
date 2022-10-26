// Fill out your copyright notice in the Description page of Project Settings.


#include "PCTech.h"
#include "Net/UnrealNetwork.h"

UPCTech::UPCTech() : Super()
{
}

bool UPCTech::ApplyTech(AActor* ActorToApply)
{
	if (CanApplyTech(ActorToApply))
	{
		// Code necessary to apply the tech in C++.

		BP_ApplyTech(ActorToApply);

		return true;
	}

	return false;
}

bool UPCTech::CanApplyTech(AActor* ActorToApply)
{
	// Other necessary comprobations to apply this tech in C++.

	return BP_CanApplyTech(ActorToApply);
}

bool UPCTech::BP_CanApplyTech_Implementation (AActor* ActorToApply)
{
	// This function is meant to be implemented in blueprints.
	// The only reason why this exist is because we need to
	// return true by default in case the blueprint doesn't
	// want to implement it.

	return true;
}

void UPCTech::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPCTech, Owner);
}
