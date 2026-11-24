// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUSettingsRegistry.h"

#include "WUSettingsRuntime/Logging/WUSettingsLog.h"

FWUSettingsRegistry& FWUSettingsRegistry::Get()
{
	static FWUSettingsRegistry Instance;
	return Instance;
}

void FWUSettingsRegistry::RegisterSetting(const FWUSettingDescriptor& Descriptor)
{
	if (Descriptor.SettingName.IsNone())
	{
		UE_LOG(LogWUSettings, Error, TEXT("RegisterSetting: descritor without SettingName was ignored"));
		return;
	}

	if (Descriptor.DefaultValue.Type != Descriptor.Type)
	{
		UE_LOG(LogWUSettings, Error, TEXT("RegisterSetting: '%s' DefaultValue type does not match descriptor Type, ignored"), *Descriptor.SettingName.ToString());
		return;
	}

	FWUSettingDescriptor* Existing = Settings.FindByPredicate(
		[&Descriptor](const FWUSettingDescriptor& Other)
		{
			return Other.SettingName == Descriptor.SettingName;
		});

	if (Existing)
	{
		UE_LOG(LogWUSettings, Log, TEXT("RegisterSetting: '%s' already registered, overriding"), *Descriptor.SettingName.ToString());
		*Existing = Descriptor;
		return;
	}

	Settings.Add(Descriptor);
}

void FWUSettingsRegistry::Reset()
{
	Settings.Reset();
}

const FWUSettingDescriptor* FWUSettingsRegistry::FindSetting(FName SettingName) const
{
	return Settings.FindByPredicate(
		[SettingName](const FWUSettingDescriptor& Descriptor)
		{
			return Descriptor.SettingName == SettingName;
		});
}

const TArray<FWUSettingDescriptor>& FWUSettingsRegistry::GetAllSettings() const
{
	return Settings;
}

bool FWUSettingsRegistry::GetDefaultValue(FName SettingName, FWUSettingValue& OutValue) const
{
	if (const FWUSettingDescriptor* Descriptor = FindSetting(SettingName))
	{
		OutValue = Descriptor->DefaultValue;
		return true;
	}
	return false;
}
