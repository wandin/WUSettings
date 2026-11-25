// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUSettingsSubsystem.h"

#include <rapidjson/document.h>

#include "Kismet/GameplayStatics.h"
#include "VerseVM/VVMVerseEnum.h"
#include "WUSettingsRuntime/Appliers/WUSettingApplier.h"
#include "WUSettingsRuntime/Appliers/WUSettingsApplierRegistry.h"
#include "WUSettingsRuntime/Logging/WUSettingsLog.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsRegistry.h"


void UWUSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadSettings();
	// No world exists yet, UWUSettingsWorldSubsystem applies everything on each world BeginPlay.
}

bool UWUSettingsSubsystem::GetSettingValue(FName SettingName, FWUSettingValue& OutValue) const
{
	if (const FWUSettingValue* Cached = CachedValues.Find(SettingName))
	{
		OutValue = *Cached;
		return true;
	}

	return FWUSettingsRegistry::Get().GetDefaultValue(SettingName, OutValue);
}

bool UWUSettingsSubsystem::SetSettingValue(FName SettingName, const FWUSettingValue& Value)
{
	const FWUSettingDescriptor* Descriptor = FWUSettingsRegistry::Get().FindSetting(SettingName);
	if (!Descriptor)
	{
		UE_LOG(LogWUSettings, Warning, TEXT("SetSettingValue: unkown setting '%s'"), *SettingName.ToString());
		return false;
	}

	FWUSettingValue Sanitized;
	if (!SanitizeValue(*Descriptor, Value, Sanitized))
	{
		UE_LOG(LogWUSettings, Warning, TEXT("SetSettingValue: invalid value for '%s' (expected %s, got %s)"), *SettingName.ToString(), *UEnum::GetValueAsString(Descriptor->Type), *UEnum::GetValueAsString(Value.Type));
		return false;
	}

	CachedValues.Add(SettingName, Sanitized);

	if (UWorld* World = GetWorld())
	{
		ApplySetting(World, *Descriptor, Sanitized);
	}

	// Always save, even without a world
	SaveSettings();
	return true;
}

void UWUSettingsSubsystem::SaveSettings()
{
	if (!SaveGameObject)
	{
		UE_LOG(LogWUSettings, Error, TEXT("SaveSettings: no SaveGame object; settings not saved"));
		return;
	}

	SaveGameObject->Version = UWUSettingSaveGame::CurrentVersion;
	SaveGameObject->StoredSettings = CachedValues;

	if (!UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveSlot, UserIndex))
	{
		UE_LOG(LogWUSettings, Error, TEXT("Failed to save settings to slot '%s'"), SaveSlot);
	}
}

void UWUSettingsSubsystem::ApplyAllRegisteredSettings(UWorld* World)
{
	if (!World)
	{
		return;
	}

	for (const FWUSettingDescriptor& Descriptor : FWUSettingsRegistry::Get().GetAllSettings())
	{
		FWUSettingValue Value;
		if (GetSettingValue(Descriptor.SettingName, Value))
		{
			ApplySetting(World, Descriptor, Value);
		}
	}

	UE_LOG(LogWUSettings, Log, TEXT("Applied all settings to world '%s'"), *World->GetName());
}

void UWUSettingsSubsystem::LoadSettings()
{
	SaveGameObject = nullptr;
	CachedValues.Reset();

	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, UserIndex))
	{
		SaveGameObject = Cast<UWUSettingSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, UserIndex));

		if (!SaveGameObject)
		{
			UE_LOG(LogWUSettings, Warning, TEXT("Save slot '%s' could not be loaded; using defaults"), SaveSlot);
		}
		else if (SaveGameObject->Version > UWUSettingSaveGame::CurrentVersion)
		{
			UE_LOG(LogWUSettings, Warning, TEXT("Save version %d is newer than supported %d; using defaults"), SaveGameObject->Version, UWUSettingSaveGame::CurrentVersion);
			SaveGameObject = nullptr;
		}
	}

	if (!SaveGameObject)
	{
		SaveGameObject = Cast<UWUSettingSaveGame>(UGameplayStatics::CreateSaveGameObject(UWUSettingSaveGame::StaticClass()));
		return;
	}

	// Future migrations (Version < CurrentVersion) go here.

	const FWUSettingsRegistry& Registry = FWUSettingsRegistry::Get();
	for (const TPair<FName, FWUSettingValue>& Pair : SaveGameObject->StoredSettings)
	{
		const FWUSettingDescriptor* Descriptor = Registry.FindSetting(Pair.Key);
		if (!Descriptor)
		{
			UE_LOG(LogWUSettings, Log, TEXT("Dropping unkown saved setting '%s'"), *Pair.Key.ToString());
			continue;
		}

		FWUSettingValue Sanitized;
		if (!SanitizeValue(*Descriptor, Pair.Value, Sanitized))
		{
			UE_LOG(LogWUSettings, Warning, TEXT("Dropping saved setting '%s': type no longer matches"), *Pair.Key.ToString());
			continue;
		}
		CachedValues.Add(Pair.Key, Sanitized);
	}

	UE_LOG(LogWUSettings, Log, TEXT("Loaded %d saved settings."), CachedValues.Num());
}

void UWUSettingsSubsystem::ApplySetting(UWorld* World, const FWUSettingDescriptor& Descriptor,
	const FWUSettingValue& Value) const
{
	if (IWUSettingApplier* Applier = FWUSettingsApplierRegistry::Get().FindApplier(Descriptor))
	{
		Applier->Apply(World, Descriptor, Value);
	}
}

bool UWUSettingsSubsystem::SanitizeValue(const FWUSettingDescriptor& Descriptor, const FWUSettingValue& InValue,
	FWUSettingValue& OutValue)
{
	if (InValue.Type != Descriptor.Type)
	{
		return false;
	}

	OutValue = InValue;

	switch (Descriptor.Type)
	{
	case EWUSettingType::Float:
		if (!FMath::IsFinite(InValue.FloatValue))
		{
			return false;
		}
		OutValue.FloatValue = FMath::Clamp(InValue.FloatValue, Descriptor.MinValue, Descriptor.MaxValue);
		break;

	case EWUSettingType::Int:
		OutValue.IntValue = FMath::Clamp(InValue.IntValue, FMath::RoundToInt(Descriptor.MinValue), FMath::RoundToInt(Descriptor.MaxValue));
		break;
	case EWUSettingType::Enum:
		// Enum descriptors must set MinValue/MaxValue to the valid index range.
		// Out-of-range is rejected (not clamped): clamping an enum picks an arbitrary option.
		if (InValue.EnumValue < FMath::RoundToInt(Descriptor.MinValue) || InValue.EnumValue > FMath::RoundToInt(Descriptor.MaxValue))
		{
			return false;
		}
		break;
	default:
		// Bool / Resolution: nothing to validate here.
		break;
	}
	return true;
}
