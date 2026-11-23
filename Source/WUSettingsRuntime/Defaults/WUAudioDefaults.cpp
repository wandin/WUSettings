// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUSettingsRuntime/SettingsCore/WUSettingDescriptor.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsRegistry.h"
#include "WUSettingsRuntime/Defaults/WUSettingsDefaults.h"

void WUSettingsDefaults::RegisterAudioSettings()
{
	auto RegisterFloatVolume = [](FName Name, const TCHAR* Display, const TCHAR* Description)
	{
		FWUSettingDescriptor Desc;
		Desc.SettingName    = Name;
		Desc.DisplayName    = FText::FromString(Display);
		Desc.Description    = FText::FromString(Description);
		Desc.Category       = EWUSettingCategory::Audio;
		Desc.Type           = EWUSettingType::Float;
		Desc.ApplyStrategy  = EWUSettingApplyStrategy::Immediate;
		Desc.MinValue       = 0.0f;
		Desc.MaxValue       = 1.0f;
		Desc.Step           = 0.01f;
		Desc.DefaultValue   = FWUSettingValue::MakeFloat(1.0f);

		FWUSettingsRegistry::Get().RegisterSetting(Desc);
	};

	RegisterFloatVolume(
		FName("Audio.MasterVolume"),
		TEXT("Master Volume"),
		TEXT("Controls overall audio volume."));

	RegisterFloatVolume(
		FName("Audio.MusicVolume"),
		TEXT("Music Volume"),
		TEXT("Controls music volume."));

	RegisterFloatVolume(
		FName("Audio.SFXVolume"),
		TEXT("SFX Volume"),
		TEXT("Controls sound effects volume."));

	RegisterFloatVolume(
		FName("Audio.VoiceVolume"),
		TEXT("Voice Volume"),
		TEXT("Controls voice audio volume."));

	RegisterFloatVolume(
		FName("Audio.UIVolume"),
		TEXT("UI Volume"),
		TEXT("Controls user interface audio volume."));
}