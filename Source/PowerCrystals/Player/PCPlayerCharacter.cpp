// Fill out your copyright notice in the Description page of Project Settings.

#include "PCPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "../Abilities/PCAbilitySystemComponent.h"
#include "../Player/PCPlayerController.h"
#include "../TechTree/PCTechTreeSystemComponent.h"
#include "../TechTree/PCTech.h"
#include "../Units/PCUnit.h"
#include "../Buildings/PCBuilding.h"

APCPlayerCharacter::APCPlayerCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 3000.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	AbilitySystem = CreateDefaultSubobject<UPCAbilitySystemComponent>("AbilitySystem");

	TechTreeSystem = CreateDefaultSubobject<UPCTechTreeSystemComponent>("TechTreeSystem");

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	CameraZoomSpeed = 20.0f;
	CameraRotationSpeed = 1.0f;
	MaxCameraRotationSpeed = 10.0f;
	CameraRotationDecelerateSpeed = 2.0f;
	CurrentCameraRotationSpeed = 0.0f;
}

void APCPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (CursorToWorld && PC)
	{
		FHitResult TraceHitResult;
		PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
		FVector CursorFV = TraceHitResult.ImpactNormal;
		FRotator CursorR = CursorFV.Rotation();
		CursorToWorld->SetWorldLocation(TraceHitResult.Location);
		CursorToWorld->SetWorldRotation(CursorR);
	}

	if (!FMath::IsNearlyZero(CurrentCameraRotationSpeed))
	{
		FRotator Rotation = FRotator(0.0f, CurrentCameraRotationSpeed * DeltaSeconds, 0.0f);
		AddActorWorldRotation(Rotation);

		CurrentCameraRotationSpeed = FMath::Lerp(CurrentCameraRotationSpeed, 0.0f, DeltaSeconds * CameraRotationDecelerateSpeed);
	}
}

bool APCPlayerCharacter::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bReturn = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (UPCTech* PCTech : TechTreeSystem->GetActiveTechs())
	{
		bReturn |= Channel->ReplicateSubobject(PCTech, *Bunch, *RepFlags);
	}

	return bReturn;
}

void APCPlayerCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorRotation().Vector(), Value);
}

void APCPlayerCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void APCPlayerCharacter::Zoom(float Value)
{
	CameraBoom->TargetArmLength += Value * CameraZoomSpeed;
}

void APCPlayerCharacter::AddCameraRotation(float Value)
{
	CurrentCameraRotationSpeed += Value * CameraRotationSpeed;
	CurrentCameraRotationSpeed = FMath::Clamp(CurrentCameraRotationSpeed, MaxCameraRotationSpeed * -1.0f, MaxCameraRotationSpeed);
}

bool APCPlayerCharacter::ExecuteAbility(FGameplayTag AbilityTag, FHitResult Hit)
{
	if (!AbilitySystem)
	{
		return false;
	}

	APCPlayerController* PC = Cast<APCPlayerController>(GetController());
	if (!PC)
	{
		return false;
	}

	CancelCurrentAbility();

	return AbilitySystem->ActivateAbility(AbilityTag, Hit, PC);
}

void APCPlayerCharacter::CancelCurrentAbility()
{
	if (!AbilitySystem)
	{
		return;
	}

	APCPlayerController* PC = Cast<APCPlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	AbilitySystem->CancelCurrentAbility(PC);
}

APCBuilding* APCPlayerCharacter::SpawnBuilding(TSubclassOf<APCBuilding> BuildingBlueprint, FTransform StartTransform, bool WithPreview)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APCPlayerController* PlayerController = Cast<APCPlayerController>(GetController());
	if (!PlayerController)
	{
		return nullptr;
	}

	APCBuilding* NewBuilding = World->SpawnActorDeferred<APCBuilding>(BuildingBlueprint, StartTransform, this);
	if (NewBuilding)
	{
		NewBuilding->Team = PlayerController->Team;
		NewBuilding->Faction = PlayerController->Faction;
		NewBuilding->PlayerOwner = PlayerController;
		NewBuilding->HasPreview = WithPreview;
		NewBuilding->FinishSpawning(StartTransform);

		FScriptDelegate OnUnitSpawnedDelegate = FScriptDelegate();
		OnUnitSpawnedDelegate.BindUFunction(this, "OnUnitSpawned");
		NewBuilding->OnUnitSpawned.Add(OnUnitSpawnedDelegate);

		AddNewPlayerBuilding(NewBuilding);

		TechTreeSystem->ApplyTechTree(NewBuilding);
	}

	return NewBuilding;
}

void APCPlayerCharacter::OnUnitSpawned(AActor* BuildingSpawner, AActor* NewUnit)
{
	APCUnit* Unit = Cast<APCUnit>(NewUnit);
	if (Unit)
	{
		AddNewPlayerUnit(Unit);

		TechTreeSystem->ApplyTechTree(Unit);
	}
}

void APCPlayerCharacter::AddNewPlayerUnit(APCUnit* NewUnit)
{
	if (NewUnit)
	{
		PlayerUnits.Add(NewUnit);
	}
}

void APCPlayerCharacter::RemovePlayerUnit(APCUnit* UnitToRemove)
{
	if (UnitToRemove)
	{
		PlayerUnits.Remove(UnitToRemove);
	}
}

void APCPlayerCharacter::AddNewPlayerBuilding(APCBuilding* NewBuilding)
{
	if (NewBuilding)
	{
		PlayerBuildings.Add(NewBuilding);
	}
}

void APCPlayerCharacter::RemovePlayerBuilding(APCBuilding* BuildingToRemove)
{
	if (BuildingToRemove)
	{
		PlayerBuildings.Remove(BuildingToRemove);
	}
}

TArray<APCUnit*> APCPlayerCharacter::GetPlayerUnits() const
{
	return PlayerUnits;
}

TArray<APCBuilding*> APCPlayerCharacter::GetPlayerBuildings() const
{
	return PlayerBuildings;
}

TArray<AActor*> APCPlayerCharacter::GetPlayerActionableActors() const
{
	TArray<AActor*> ReturnActors;

	ReturnActors.Append(PlayerUnits);
	ReturnActors.Append(PlayerBuildings);

	return ReturnActors;
}

void APCPlayerCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APCPlayerCharacter, PlayerUnits);
	DOREPLIFETIME(APCPlayerCharacter, PlayerBuildings);
}
