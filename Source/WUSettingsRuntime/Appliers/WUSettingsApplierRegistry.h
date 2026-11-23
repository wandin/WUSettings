// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once
#include "WUSettingApplier.h"

class WUSETTINGSRUNTIME_API FWUSettingsApplierRegistry
{
public:
	static FWUSettingsApplierRegistry& Get();

	void RegisterApplier(TUniquePtr<IWUSettingApplier> Applier);
	IWUSettingApplier* FindApplier(const FWUSettingDescriptor& Descriptor);

	// Removes all registered appliers (module shutdown).
	void Reset();

private:

	FWUSettingsApplierRegistry() = default;
	~FWUSettingsApplierRegistry() = default;

	// Forbid copy and/or move
	FWUSettingsApplierRegistry(const FWUSettingsApplierRegistry&) = delete;
	FWUSettingsApplierRegistry& operator=(const FWUSettingsApplierRegistry&) = delete;
	FWUSettingsApplierRegistry(FWUSettingsApplierRegistry&&) = delete;
	FWUSettingsApplierRegistry& operator=(FWUSettingsApplierRegistry&&) = delete;


	TArray<TUniquePtr<IWUSettingApplier>> Appliers;
};
