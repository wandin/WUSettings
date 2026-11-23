// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WUSettingsTypes.h"
#include "WUSettingsRuntime/Persistence/WUSettingValue.h"

#include "WUSettingDescriptor.generated.h"


USTRUCT(BlueprintType)
struct FWUSettingDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SettingName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWUSettingCategory Category;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWUSettingType Type;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWUSettingApplyStrategy ApplyStrategy;

	// Generic numeric constraints
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinValue = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxValue = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Step = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWUSettingValue DefaultValue;
};