// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUSettingsSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "WUSettingsRuntime/Appliers/WUSettingApplier.h"
#include "WUSettingsRuntime/Appliers/WUSettingsApplierRegistry.h"
#include "WUSettingsRuntime/Logging/WUSettingsLog.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsRegistry.h"

void UWUSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadSettings();
	ApplyAllRegisteredSettings();
}

void UWUSettingsSubsystem::SaveSettings()
{
	if (!SaveGameObject)
	{
		return;
	}

	SaveGameObject->StoredSettings = CachedValues;
	UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveSlot, 0);
}

void UWUSettingsSubsystem::ApplyAllRegisteredSettings()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	const auto& All = FWUSettingsRegistry::Get().GetAllSettings();

	for (const FWUSettingDescriptor& Descriptor : All)
	{
		FWUSettingValue Value;
		if (GetSettingValue(Descriptor.SettingName, Value))
		{
			if (IWUSettingApplier* Applier = FWUSettingsApplierRegistry::Get().FindApplier(Descriptor))
			{
				Applier->Apply(World, Descriptor, Value);
			}
		}
	}
}

void UWUSettingsSubsystem::SetAudioVolume(const FString& SettingName, float Value)
{
	const FName Name(*SettingName);

	SetSettingValue(Name, FWUSettingValue::MakeFloat(Value));

	UE_LOG(LogWUSettings, Log, TEXT("WUSettings: %s set to %f"), *SettingName, Value);
}

void UWUSettingsSubsystem::LoadSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, 0))
	{
		SaveGameObject = Cast<UWUSettingSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, 0));
	}

	if (!SaveGameObject)
	{
		SaveGameObject = Cast<UWUSettingSaveGame>(UGameplayStatics::CreateSaveGameObject(UWUSettingSaveGame::StaticClass()));
	}

	CachedValues = SaveGameObject->StoredSettings;
}

bool UWUSettingsSubsystem::GetSettingValue(FName SettingName, FWUSettingValue& OutValue)
{
	if (const FWUSettingValue* Value = CachedValues.Find(SettingName))
	{
		OutValue = *Value;
		return true;
	}

	if (FWUSettingsRegistry::Get().GetDefaultValue(SettingName, OutValue))
	{
		return true;
	}
	return false;
}

void UWUSettingsSubsystem::SetSettingValue(FName SettingName, const FWUSettingValue& Value)
{
	CachedValues.Add(SettingName, Value);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	const FWUSettingDescriptor* Descriptor = FWUSettingsRegistry::Get().FindSetting(SettingName);
	if (Descriptor)
	{
		if (IWUSettingApplier* Applier = FWUSettingsApplierRegistry::Get().FindApplier(*Descriptor))
		{
			Applier->Apply(World, *Descriptor, Value);
		}
	}
	SaveSettings();
}
