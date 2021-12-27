// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PCBuilding.generated.h"

UCLASS()
class POWERCRYSTALS_API APCBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APCBuilding();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns ability system component. **/
	FORCEINLINE class UPCAbilitySystemComponent* GetAbilitySystem() { return AbilitySystem; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** The ability system component. */
	UPROPERTY(Category = Building, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPCAbilitySystemComponent* AbilitySystem;

	/** The main skeletal mesh associated with this building. */
	UPROPERTY(Category = Building, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh;

	/** The BoxComponent being used for collision. */
	UPROPERTY(Category = Building, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;

};
