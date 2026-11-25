// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once
#include "WUSettingsRuntime/Appliers/WUSettingApplier.h"


class FWUVideoSettingApplier : public IWUSettingApplier
{
public:
	virtual bool CanApply(const FWUSettingDescriptor& Descriptor) const override;
	virtual void Apply(UWorld* World, const FWUSettingDescriptor& Descriptor, const FWUSettingValue& Value) override;

private:
	void ApplyVSync(bool bEnabled);
	void ApplyFOV(UWorld* World, float FOV);

	void ApplyWindowMode(UWorld*  World, int32 ModeIndex);
};
