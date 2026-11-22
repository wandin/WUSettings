#pragma once

#include "WuSettingsRuntime/Appliers/WUSettingApplier.h"

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
	
	TWeakObjectPtr<UWUAudioApplierConfig> ConfigAsset;
	TWeakObjectPtr<USoundMix> Mix;

	// Cache: SettingName -> SoundClass*
	TMap<FName, TWeakObjectPtr<USoundClass>> CachedClasses;

	bool bPushedMix = false;
};
