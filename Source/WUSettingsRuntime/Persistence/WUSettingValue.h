// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsTypes.h"

#include "WUSettingValue.generated.h"

USTRUCT(BlueprintType)
struct WUSETTINGSRUNTIME_API FWUSettingValue
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

	// (0,0) = native desktop resolution, resolved when applied.
	UPROPERTY()
	FIntPoint ResolutionValue = FIntPoint::ZeroValue;
	
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

	static FWUSettingValue MakeResolution(int32 Width, int32 Height)
	{
		FWUSettingValue Result;
		Result.Type = EWUSettingType::Resolution;
		Result.ResolutionValue = FIntPoint(Width, Height);
		return Result;
	}

	static FWUSettingValue MakeNativeResolution()
	{
		return MakeResolution(0,0);
	}

	bool operator == (const FWUSettingValue& Other) const
	{
		if (Type != Other.Type)
		{
			return false;
		}

		switch (Type)
		{
			case EWUSettingType::Bool:			return BoolValue == Other.BoolValue;
			case EWUSettingType::Int:			return IntValue == Other.IntValue;
			case EWUSettingType::Float:			return FMath::IsNearlyEqual(FloatValue, Other.FloatValue);
			case EWUSettingType::Enum:			return EnumValue == Other.EnumValue;
			case EWUSettingType::Resolution:	return ResolutionValue == Other.ResolutionValue;
			default:							return false;
		}
	}
};
