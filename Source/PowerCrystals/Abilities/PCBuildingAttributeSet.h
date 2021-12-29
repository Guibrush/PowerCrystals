// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "PCBuildingAttributeSet.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class POWERCRYSTALS_API UPCBuildingAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Health")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Health")
	float Health;

private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
