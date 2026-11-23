// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUAudioSettingApplier.h"

#include "WUAudioApplierConfig.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "WUSettingsRuntime/Logging/WUSettingsLog.h"
#include "WUSettingsRuntime/Settings/WUSettingsDeveloperSettings.h"

bool FWUAudioSettingApplier::CanApply(const FWUSettingDescriptor& Descriptor) const
{
	return Descriptor.Category == EWUSettingCategory::Audio;
}

void FWUAudioSettingApplier::EnsureLoaded()
{
	if (ConfigAsset.IsValid())
	{
		return;
	}

	const UWUSettingsDeveloperSettings* Settings = GetDefault<UWUSettingsDeveloperSettings>();
	if (!Settings)
	{
		UE_LOG(LogWUSettingsAudio, Error, TEXT("WUAudio: DeveloperSettings not found"));
		return;
	}

	UWUAudioApplierConfig* LoadedConfig = Settings->AudioApplierConfig.LoadSynchronous();
	if (!LoadedConfig)
	{
        UE_LOG(LogWUSettingsAudio, Error, TEXT("WUAudio: Failed to load AudioApplierConfig"));
		return;
	}
	UE_LOG(LogWUSettingsAudio, Log, TEXT("WUAudio: AudioApplierConfig loaded"));

	ConfigAsset = LoadedConfig;
	CachedClasses.Reset();

	if (USoundMix* LoadedMix = LoadedConfig->SoundMix.LoadSynchronous())
	{
		Mix = LoadedMix;
	}

	for (const FWUAudioSettingSoundClassMap& Map : LoadedConfig->Maps)
	{
		if (USoundClass* SoundClass = Map.SoundClass.LoadSynchronous())
		{
			CachedClasses.Add(Map.SettingName, SoundClass);
		}
	}
}

USoundClass* FWUAudioSettingApplier::ResolveSoundClass(FName SettingName)
{
	if (const TWeakObjectPtr<USoundClass>* FoundClass = CachedClasses.Find(SettingName))
	{
		return FoundClass->Get();
	}
	return nullptr;
}

void FWUAudioSettingApplier::Apply(UWorld* World, const FWUSettingDescriptor& Descriptor, const FWUSettingValue& Value)
{
	if (!World || Descriptor.Type != EWUSettingType::Float)
	{
		return;
	}

	EnsureLoaded();

	UWUAudioApplierConfig* Config = ConfigAsset.Get();
	USoundMix* SoundMix = Mix.Get();
	USoundClass* SoundClass = ResolveSoundClass(Descriptor.SettingName);

	if (!Config || !SoundMix || !SoundClass)
	{
		return;
	}

	const float Volume = FMath::Clamp(Value.FloatValue, 0.0f, 1.0f);

	// 1️⃣ Apply override FIRST
	UGameplayStatics::SetSoundMixClassOverride(
		World,
		SoundMix,
		SoundClass,
		Volume,
		1.0f,                 // Pitch
		Config->FadeInTime,
		true                  // Apply to children
	);

	// 2️⃣ Force engine to re-evaluate the mix
	UGameplayStatics::PushSoundMixModifier(World, SoundMix);
}
