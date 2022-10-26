// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PCTechTreeSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POWERCRYSTALS_API UPCTechTreeSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPCTechTreeSystemComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void AddNewTech(TSubclassOf<class UPCTech> NewTech);

	UFUNCTION(BlueprintCallable)
	void ApplyTechTree(AActor* ActorToApply);

	UFUNCTION(BlueprintPure)
	TArray<class UPCTech*> GetActiveTechs() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<class UPCTech*> ActiveTechs;

private:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

		
};
