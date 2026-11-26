// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once
#include "WUSettingsRuntime/Appliers/WUSettingApplier.h"

class UGameUserSettings;

class FWUVideoSettingApplier : public IWUSettingApplier
{
public:
	virtual bool CanApply(const FWUSettingDescriptor& Descriptor) const override;
	virtual void Apply(UWorld* World, const FWUSettingDescriptor& Descriptor, const FWUSettingValue& Value) override;

private:
	void ApplyVSync(bool bEnabled);
	void ApplyFrameRateLimit(int32 Limit);
	void ApplyFOV(UWorld* World, float FOV);

	/**
	* Window mode and resolution each update their part in GameUserSettings,
	* Then ApplyDisplaySettings applies the combined state once.
	*/
	void ApplyWindowMode(UWorld*  World, int32 ModeIndex);
	void ApplyResolution(UWorld* World, const FIntPoint& Resolution);

	void ApplyDisplaySettings(UGameUserSettings* Settings);
};
