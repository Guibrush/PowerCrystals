// Fill out your copyright notice in the Description page of Project Settings.


#include "PCBuilding.h"
#include "../Abilities/PCAbilitySystemComponent.h"
#include "../Abilities/PCAttributeSet.h"
#include "../Components/PCActionableActorComponent.h"
#include "../Player/PCPlayerController.h"
#include "../Tasks/PCTaskSystemComponent.h"
#include "../Units/PCUnit.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavAreas/NavArea_Null.h"
#include "NavAreas/NavArea_Default.h"
#include "Engine/ActorChannel.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryOption.h"
#include "EnvironmentQuery/Tests/EnvQueryTest_Overlap.h"

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

	TaskSystem = CreateDefaultSubobject<UPCTaskSystemComponent>("TaskSystem");

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

bool APCBuilding::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bReturn = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (UPCTask* PCTask : TaskSystem->GetActiveTasks())
	{
		bReturn |= Channel->ReplicateSubobject(PCTask, *Bunch, *RepFlags);
	}

	return bReturn;
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

bool APCBuilding::InitConstructionMode()
{
	if (IsInPreview && !IsInConstruction && ValidPosition)
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

		return true;
	}

	return false;
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

bool APCBuilding::ExecuteAbility(FGameplayTag InputAbilityTag, FHitResult Hit)
{
	if (IsDestroyed || !AbilitySystem)
	{
		return false;
	}

	return AbilitySystem->ActivateAbility(InputAbilityTag, Hit, PlayerOwner);
}

bool APCBuilding::SpawnPlayerUnits(TArray<TSubclassOf<APCUnit>> UnitBlueprints)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return false;
	}

	if (!HasAuthority())
	{
		return false;
	}

	if (UnitBlueprints.Num() <= 0)
	{
		return false;
	}

	TArray<UEnvQueryOption*> QueryOptions = SpawnUnitsLocationQuery->GetOptionsMutable();
	for (UEnvQueryOption* Option : QueryOptions)
	{
		for (TObjectPtr<UEnvQueryTest> Test : Option->Tests)
		{
			UEnvQueryTest_Overlap* TestOverlap = Cast<UEnvQueryTest_Overlap>(Test);
			if (TestOverlap)
			{
				TestOverlap->OverlapData.ExtentX = UnitBlueprints[0].GetDefaultObject()->GetCapsuleComponent()->GetScaledCapsuleRadius();
				TestOverlap->OverlapData.ExtentY = UnitBlueprints[0].GetDefaultObject()->GetCapsuleComponent()->GetScaledCapsuleRadius();
				TestOverlap->OverlapData.ExtentZ = UnitBlueprints[0].GetDefaultObject()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;
				TestOverlap->OverlapData.ShapeOffset = FVector(0.0f, 0.0f, TestOverlap->OverlapData.ExtentZ);
			}
		}
	}

	FEnvQueryRequest UnitsSpawnLocationQueryRequest = FEnvQueryRequest(SpawnUnitsLocationQuery, this);
	UnitsSpawnLocationQueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &APCBuilding::SpawnUnitsQueryFinished);
	UnitToSpawnBlueprints = UnitBlueprints;

	return true;
}

void APCBuilding::SpawnUnitsQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	if (!HasAuthority())
	{
		return;
	}

	if (UnitToSpawnBlueprints.Num() <= 0)
	{
		return;
	}

	if (!Result)
	{
		return;
	}

	TArray<FVector> ResultLocations;
	Result->GetAllAsLocations(ResultLocations);
	if (ResultLocations.Num() > 0)
	{
		int32 index = 0;
		for (TSubclassOf<APCUnit> UnitBlueprint : UnitToSpawnBlueprints)
		{
			if (ResultLocations.Num() > index)
			{
				FVector UnitLocation = ResultLocations[index];
				FRotator UnitRotation = UnitsSpawnPoint->GetComponentRotation();
				FTransform StartTransform = FTransform(UnitRotation, UnitLocation);
				APCUnit* NewUnit = World->SpawnActorDeferred<APCUnit>(UnitBlueprint, StartTransform, PlayerOwner, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
				if (NewUnit)
				{
					NewUnit->Team = Team;
					NewUnit->Faction = Faction;
					NewUnit->PlayerOwner = PlayerOwner;
					NewUnit->FinishSpawning(StartTransform);
				}

				index++;
			}
			else
			{
				break;
			}
		}
	}
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
