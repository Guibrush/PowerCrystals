// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PCTech.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class POWERCRYSTALS_API UPCTech : public UObject
{
	GENERATED_BODY()

public:

	UPCTech();

	UPROPERTY(BlueprintReadOnly, Replicated)
	class AActor* Owner;

	bool ApplyTech(AActor* ActorToApply);

	bool CanApplyTech(AActor* ActorToApply);

	UFUNCTION(BlueprintNativeEvent, Category = PCTask, DisplayName = "CanApplyTech", meta = (ScriptName = "CanApplyTech"))
	bool BP_CanApplyTech(AActor* ActorToApply);

	UFUNCTION(BlueprintImplementableEvent, Category = PCTech, DisplayName = "ApplyTech", meta = (ScriptName = "ApplyTech"))
	void BP_ApplyTech(AActor* ActorToApply);

private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
};
