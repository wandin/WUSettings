// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "WUSettingsRuntime/Appliers/WUSettingApplier.h"
#include "UObject/StrongObjectPtr.h"

class UWUAudioApplierConfig;
class USoundClass;
class USoundMix;

class FWUAudioSettingApplier : public IWUSettingApplier
{
public:
	virtual bool CanApply(const FWUSettingDescriptor& Descriptor) const override;
	virtual void Apply(UWorld* World, const FWUSettingDescriptor& Descriptor, const FWUSettingValue& Value) override;

private:
	void EnsureLoaded();
	USoundClass* ResolveSoundClass(FName SettingName);
	
	// Strong refs: this class isn't a UObject, so weak pointers could be garbage collected.
	TStrongObjectPtr<UWUAudioApplierConfig> ConfigAsset;
	TStrongObjectPtr<USoundMix> Mix;
	TMap<FName, TStrongObjectPtr<USoundClass>> CachedClasses;

	// World where the mix was last pushed. Pushing on every Apply stacks modifiers.
	TWeakObjectPtr<UWorld> MixPushedWorld;
};
