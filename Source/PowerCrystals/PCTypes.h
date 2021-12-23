// Copyright 2019-2020 Alberto & co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PCTypes.generated.h"

UENUM(BlueprintType)
enum class EPCTeamType : uint8
{
	NoTeam = 0,
	AI = 1,
	Team1 = 2,
	Team2 = 3,
	Team3 = 4,
	Team4 = 5,
	Neutral,
};
