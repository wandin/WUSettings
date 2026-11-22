#include "WUSettingsRegistry.h"

FWUSettingsRegistry& FWUSettingsRegistry::Get()
{
	static FWUSettingsRegistry Instance;
	return Instance;
}

void FWUSettingsRegistry::RegisterSetting(const FWUSettingDescriptor& Descriptor)
{
	Settings.Add(Descriptor);
}

const FWUSettingDescriptor* FWUSettingsRegistry::FindSetting(FName SettingName) const
{
	return Settings.FindByPredicate(
		[&](const FWUSettingDescriptor& Descriptor)
		{
			return Descriptor.SettingName == SettingName;
		});
}

const TArray<FWUSettingDescriptor>& FWUSettingsRegistry::GetAllSettings() const
{
	return Settings;
}

bool FWUSettingsRegistry::GetDefaultValue(FName SettingName, FWUSettingValue& OutValue)
{
	if (const FWUSettingDescriptor* Descriptor = FindSetting(SettingName))
	{
		OutValue = Descriptor->DefaultValue;
		return true;
	}
	return false;
}
