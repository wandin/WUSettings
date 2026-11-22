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

/*
 * Resolution
 */
USTRUCT(BlueprintType)
struct FWUResolution
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Width = 1920;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Height = 1080;

	bool operator==(const FWUResolution& Other) const
	{
		return Width == Other.Width && Height == Other.Height;
	}
};