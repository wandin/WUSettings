// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WUSettingDescriptor.h"

class WUSETTINGSRUNTIME_API FWUSettingsRegistry
{
public:
	static FWUSettingsRegistry& Get();

	void RegisterSetting(const FWUSettingDescriptor& Descriptor);
	const FWUSettingDescriptor* FindSetting(FName SettingName) const;
	const TArray<FWUSettingDescriptor>& GetAllSettings() const;

	bool GetDefaultValue(FName SettingName, FWUSettingValue& OutValue);

private:
	TArray<FWUSettingDescriptor> Settings;
};
