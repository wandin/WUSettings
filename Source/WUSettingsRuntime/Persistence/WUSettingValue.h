// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsTypes.h"

#include "WUSettingValue.generated.h"

USTRUCT(BlueprintType)
struct FWUSettingValue
{
	GENERATED_BODY()

	UPROPERTY()
	EWUSettingType Type = EWUSettingType::Bool;

	UPROPERTY()
	bool BoolValue = false;

	UPROPERTY()
	int32 IntValue = 0;

	UPROPERTY()
	float FloatValue = 0.0f;
	
	UPROPERTY()
	int32 EnumValue = 0;
	
	static FWUSettingValue MakeBool(bool Value)
	{
		FWUSettingValue Result;
		Result.Type = EWUSettingType::Bool;
		Result.BoolValue = Value;
		return Result;
	}

	static FWUSettingValue MakeInt(int32 Value)
	{
		FWUSettingValue Result;
		Result.Type = EWUSettingType::Int;
		Result.IntValue = Value;
		return Result;
	}

	static FWUSettingValue MakeFloat(float Value)
	{
		FWUSettingValue Result;
		Result.Type = EWUSettingType::Float;
		Result.FloatValue = Value;
		return Result;
	}

	static FWUSettingValue MakeEnum(int32 Value)
	{
		FWUSettingValue Result;
		Result.Type = EWUSettingType::Enum;
		Result.EnumValue = Value;
		return Result;
	}
};
