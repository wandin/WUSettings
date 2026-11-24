// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WUSettingDescriptor.h"

class WUSETTINGSRUNTIME_API FWUSettingsRegistry
{
public:
	static FWUSettingsRegistry& Get();

	// Registers a setting. If the name already exists, the new descriptor replaces it.
	// (lets project override plugin defaults).
	void RegisterSetting(const FWUSettingDescriptor& Descriptor);

	// Remove all registered settings (module shutdown).
	void Reset();
	
	const FWUSettingDescriptor* FindSetting(FName SettingName) const;
	const TArray<FWUSettingDescriptor>& GetAllSettings() const;
	bool GetDefaultValue(FName SettingName, FWUSettingValue& OutValue) const;

private:

	FWUSettingsRegistry() = default;
	~FWUSettingsRegistry() = default;

	FWUSettingsRegistry(const FWUSettingsRegistry&) = delete;
	FWUSettingsRegistry& operator=(const FWUSettingsRegistry&) = delete;
	FWUSettingsRegistry(FWUSettingsRegistry&&) = delete;
	FWUSettingsRegistry& operator=(FWUSettingsRegistry&&) = delete;
	
	TArray<FWUSettingDescriptor> Settings;
};
