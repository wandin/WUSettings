// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUSettingsRuntime/SettingsCore/WUSettingDescriptor.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsRegistry.h"
#include "WUSettingsRuntime/Video/WUVideoTypes.h"

static void RegisterVideoSettings()
{
	//Resolution
	FWUSettingDescriptor Descriptor;
	Descriptor.SettingName = "Video.Resolution";
	Descriptor.DisplayName = FText::FromString("Resolution");
	Descriptor.Description = FText::FromString("Screen resolution");
	Descriptor.Category = EWUSettingCategory::Video;
	Descriptor.Type = EWUSettingType::Enum;
	Descriptor.ApplyStrategy = EWUSettingApplyStrategy::ApplyConfirm;
	Descriptor.DefaultValue = FWUSettingValue::MakeEnum(0);
	FWUSettingsRegistry::Get().RegisterSetting(Descriptor);

	// Window Mode
	{
		FWUSettingDescriptor Desc;
		Desc.SettingName   = "Video.WindowMode";
		Desc.DisplayName  = FText::FromString("Window Mode");
		Desc.Description  = FText::FromString("Window display mode.");
		Desc.Category     = EWUSettingCategory::Video;
		Desc.Type         = EWUSettingType::Enum;
		Desc.ApplyStrategy = EWUSettingApplyStrategy::ApplyConfirm;

		Desc.DefaultValue = FWUSettingValue::MakeEnum(
			static_cast<uint8>(EWUWindowMode::Borderless));

		FWUSettingsRegistry::Get().RegisterSetting(Desc);
	}

	// VSync
	{
		FWUSettingDescriptor Desc;
		Desc.SettingName   = "Video.VSync";
		Desc.DisplayName  = FText::FromString("VSync");
		Desc.Description  = FText::FromString("Vertical synchronization.");
		Desc.Category     = EWUSettingCategory::Video;
		Desc.Type         = EWUSettingType::Bool;
		Desc.ApplyStrategy = EWUSettingApplyStrategy::Immediate;

		Desc.DefaultValue = FWUSettingValue::MakeBool(true);
		FWUSettingsRegistry::Get().RegisterSetting(Desc);
	}

	// FOV
	{
		FWUSettingDescriptor Desc;
		Desc.SettingName   = "Video.FOV";
		Desc.DisplayName  = FText::FromString("Field of View");
		Desc.Description  = FText::FromString("Camera field of view.");
		Desc.Category     = EWUSettingCategory::Video;
		Desc.Type         = EWUSettingType::Float;
		Desc.ApplyStrategy = EWUSettingApplyStrategy::Immediate;

		Desc.MinValue = 60.f;
		Desc.MaxValue = 120.f;
		Desc.Step     = 1.f;
		Desc.DefaultValue = FWUSettingValue::MakeFloat(90.f);

		FWUSettingsRegistry::Get().RegisterSetting(Desc);
	}
}

// Auto registration
static struct FWUVideoSettingsAutoRegister
{
	FWUVideoSettingsAutoRegister()
	{
		RegisterVideoSettings();
	}
} GRegisterVideoSettings;