// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WUVideoTypes.generated.h"

/*
 * Window Mode
 */
UENUM(BlueprintType)
enum class EWUWindowMode : uint8
{
	Windowed		UMETA(DisplayName="Windowed"),
	Borderless		UMETA(DisplayName="Borderless"),
	Fullscreen		UMETA(DisplayName="Fullscreen")
};
