#pragma once

#include "WuSettingsRuntime/Persistence/WUSettingValue.h"
#include "WuSettingsRuntime/SettingsCore/WUSettingDescriptor.h"

class IWUSettingApplier
{
public:
	virtual ~IWUSettingApplier() = default;

	// Can this applier handle this setting?
	virtual bool CanApply(const FWUSettingDescriptor& Descriptor) const = 0;

	// Apply value to the engine/system
	virtual void Apply(UWorld* World, const FWUSettingDescriptor& Descriptor, const FWUSettingValue& Value) = 0;
};
