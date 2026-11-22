// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "WUSettingsTypes.generated.h"

UENUM(BlueprintType)
enum class  EWUSettingCategory : uint8
{
	Audio			UMETA(DisplayName = "Audio"),
	Video			UMETA(DisplayName = "Video"),
	Graphics		UMETA(DisplayName = "Graphics"),
	Gameplay		UMETA(DisplayName = "Gameplay"),
	Input			UMETA(DisplayName = "Input"),
	Accessibility	UMETA(DisplayName = "Accessibility"),
	Network			UMETA(DisplayName = "Network"),
	Developer		UMETA(DisplayName = "Developer")
};

UENUM(BlueprintType)
enum class EWUSettingType : uint8
{
	Bool			UMETA(DisplayName = "Bool"),
	Int				UMETA(DisplayName = "Int"),
	Float			UMETA(DisplayName = "Float"),
	Enum			UMETA(DisplayName = "Enum"),
	Resolution		UMETA(DisplayName = "Resolution")
};

UENUM(BlueprintType)
enum class EWUSettingApplyStrategy : uint8
{
	Immediate,
	ApplyConfirm,
	RequiresRestart
};
