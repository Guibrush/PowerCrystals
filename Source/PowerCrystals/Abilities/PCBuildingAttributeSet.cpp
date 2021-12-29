// Fill out your copyright notice in the Description page of Project Settings.


#include "PCBuildingAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UPCBuildingAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPCBuildingAttributeSet, MaxHealth);
	DOREPLIFETIME(UPCBuildingAttributeSet, Health);
}
