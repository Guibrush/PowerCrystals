// Fill out your copyright notice in the Description page of Project Settings.


#include "PCBuilding.h"
#include "../Abilities/PCAbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
APCBuilding::APCBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystem = CreateDefaultSubobject<UPCAbilitySystemComponent>("AbilitySystem");

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->InitBoxExtent(FVector(500.0f, 500.0f, 10.0f));
	BoxComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	BoxComponent->CanCharacterStepUpOn = ECB_No;
	BoxComponent->SetShouldUpdatePhysicsVolume(true);
	BoxComponent->SetCanEverAffectNavigation(true);
	BoxComponent->bDynamicObstacle = true;
	RootComponent = BoxComponent;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	Mesh->AlwaysLoadOnClient = true;
	Mesh->AlwaysLoadOnServer = true;
	Mesh->bOwnerNoSee = false;
	Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	Mesh->bCastDynamicShadow = true;
	Mesh->bAffectDynamicIndirectLighting = true;
	Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	static FName MeshCollisionProfileName(TEXT("CharacterMesh"));
	Mesh->SetCollisionProfileName(MeshCollisionProfileName);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetupAttachment(BoxComponent);
}

// Called when the game starts or when spawned
void APCBuilding::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APCBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

