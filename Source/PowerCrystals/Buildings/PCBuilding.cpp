// Fill out your copyright notice in the Description page of Project Settings.


#include "PCBuilding.h"
#include "../Abilities/PCAbilitySystemComponent.h"
#include "../Abilities/PCAttributeSet.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "../Components/PCActionableActorComponent.h"
#include "../Player/PCPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavAreas/NavArea_Null.h"
#include "NavAreas/NavArea_Default.h"

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

	UnitsSpawnPoint = CreateDefaultSubobject<USceneComponent>("UnitsSpawnPoint");
	UnitsSpawnPoint->SetupAttachment(BoxComponent);

	UnitsRallyPoint = CreateDefaultSubobject<USceneComponent>("UnitsRallyPoint");
	UnitsRallyPoint->SetupAttachment(BoxComponent);

	ActionableActorComponent = CreateDefaultSubobject<UPCActionableActorComponent>("ActionableActorComponent");
	ActionableActorComponent->InitComponent(false, true);
	ActionableActorComponent->SetIsReplicated(true);

	IsDestroyed = false;
	IsInPreview = false;
	IsInConstruction = false;
	HasPreview = true;
	ValidPosition = true;
}

// Called when the game starts or when spawned
void APCBuilding::BeginPlay()
{
	Super::BeginPlay();

	OnBuildingHealthChangedDelegate = FScriptDelegate();
	OnBuildingHealthChangedDelegate.BindUFunction(this, "BuildingHealthChanged");
	ActionableActorComponent->OnHealthChanged.Add(OnBuildingHealthChangedDelegate);

	if (HasPreview)
	{
		InitPreviewMode();
	}
	else
	{
		BuildingConstructed();
	}
}

// Called every frame
void APCBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsInPreview)
	{
		if (!HasAuthority())
		{
			APCPlayerController* PlayerController = Cast<APCPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			if (PlayerController == PlayerOwner)
			{
				FHitResult Hit;
				// ECC_GameTraceChannel1 is the one called "BuildingPlacement". The config of this is on the DefaultEngine.ini
				PlayerOwner->GetHitResultUnderCursor(ECC_GameTraceChannel1, false, Hit);
				if (Hit.bBlockingHit)
				{
					SetActorLocation(Hit.Location);
					ServerNewMouseProjectedPoint(Hit.Location);
				}
			}
		}
		else
		{
			SetActorLocation(MouseProjectedPoint);

			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this);
			TArray<AActor*> OutActors;
			FVector BoxPos = GetActorLocation();
			BoxPos.Z += BoxComponent->GetScaledBoxExtent().Z + (BoxComponent->GetScaledBoxExtent().Z * 0.5);
			UKismetSystemLibrary::BoxOverlapActors(GetWorld(), BoxPos, BoxComponent->GetScaledBoxExtent(), ObjectTypes, nullptr, ActorsToIgnore, OutActors);

			FVector NavMeshPoint = FVector::ZeroVector;
			bool PointProjectedInNavMesh = false;
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			if (NavSys)
			{
				FNavLocation NavLoc;
				PointProjectedInNavMesh = NavSys->ProjectPointToNavigation(GetActorLocation(), NavLoc);
				NavMeshPoint = NavLoc.Location;
			}

			ValidPosition = PointProjectedInNavMesh && (OutActors.Num() == 0);
		}
	}
}

void APCBuilding::ServerNewMouseProjectedPoint_Implementation(FVector NewPoint)
{
	MouseProjectedPoint = NewPoint;
}

void APCBuilding::BuildingSelected_Implementation()
{

}

void APCBuilding::BuildingDeselected_Implementation()
{

}

void APCBuilding::BuildingHealthChanged_Implementation(float NewValue, AActor* Attacker)
{
	if (!IsDestroyed && (NewValue <= 0.0))
	{
		BuildingDestroyed(Attacker);
	}
}

void APCBuilding::BuildingDestroyed_Implementation(AActor* Killer)
{
	IsDestroyed = true;
	ActionableActorComponent->OnDied.Broadcast(Killer, this);
}

void APCBuilding::InitPreviewMode()
{
	IsInConstruction = false;
	IsInPreview = true;

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComponent->SetCanEverAffectNavigation(false);

	BPInitPreviewMode();

	MulticastNewBuildingModeAndCollision(IsInConstruction, IsInPreview, ECollisionEnabled::NoCollision);
}

void APCBuilding::InitConstructionMode()
{
	IsInConstruction = true;
	IsInPreview = false;

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComponent->SetCanEverAffectNavigation(true);

	float ConstructionTime = 0.0f;
	UPCAttributeSet* AttributeSet = Cast<UPCAttributeSet>(AbilitySystem->GetSpawnedAttributes()[0]);
	if (AttributeSet)
	{
		ConstructionTime = AttributeSet->ConstructionTime;
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APCBuilding::BuildingConstructed, ConstructionTime, false);

	BPInitConstructionMode();

	MulticastNewBuildingModeAndCollision(IsInConstruction, IsInPreview, ECollisionEnabled::QueryAndPhysics);
}

void APCBuilding::BuildingConstructed()
{
	IsInConstruction = false;
	IsInPreview = false;

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComponent->SetCanEverAffectNavigation(true);

	BPBuildingConstructed();

	MulticastNewBuildingModeAndCollision(IsInConstruction, IsInPreview, ECollisionEnabled::QueryAndPhysics);
}

void APCBuilding::MulticastNewBuildingModeAndCollision_Implementation(bool NewIsInConstruction, bool NewIsInPreview, ECollisionEnabled::Type NewCollisionMode)
{
	IsInConstruction = NewIsInConstruction;
	IsInPreview = NewIsInPreview;

	BoxComponent->SetCollisionEnabled(NewCollisionMode);

	if (!HasAuthority())
	{
		if (!IsInPreview)
		{
			Mesh->SetHiddenInGame(false);
		}
		else
		{
			APCPlayerController* PlayerController = Cast<APCPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			if (PlayerController == PlayerOwner)
			{
				Mesh->SetHiddenInGame(false);
			}
			else
			{
				Mesh->SetHiddenInGame(true);
			}
		}
	}

	BPNewBuildingMode(NewIsInConstruction, NewIsInPreview);
}

UAbilitySystemComponent* APCBuilding::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void APCBuilding::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APCBuilding, Team);
	DOREPLIFETIME(APCBuilding, Faction);
	DOREPLIFETIME(APCBuilding, PlayerOwner);
	DOREPLIFETIME(APCBuilding, IsDestroyed);
	DOREPLIFETIME(APCBuilding, IsInPreview);
	DOREPLIFETIME(APCBuilding, IsInConstruction);
	DOREPLIFETIME(APCBuilding, HasPreview);
	DOREPLIFETIME(APCBuilding, ValidPosition);
	DOREPLIFETIME(APCBuilding, MouseProjectedPoint);
}
