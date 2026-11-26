// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WUSettingsTypes.h"
#include "WUSettingsRuntime/Persistence/WUSettingValue.h"

#include "WUSettingDescriptor.generated.h"


USTRUCT(BlueprintType)
struct WUSETTINGSRUNTIME_API FWUSettingDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SettingName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWUSettingCategory Category = EWUSettingCategory::Audio;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWUSettingType Type = EWUSettingType::Bool;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWUSettingApplyStrategy ApplyStrategy = EWUSettingApplyStrategy::Immediate;

	// Generic numeric constraints
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinValue = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxValue = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Step = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWUSettingValue DefaultValue;

	// Enum only: display label per value (index == enum value). Should cover MinValue..MaxValue;
	// missing labels fall back to the number.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FText> EnumOptions;
};