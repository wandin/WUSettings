// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUVideoSettingApplier.h"

#include "GameFramework/GameUserSettings.h"
#include "WUSettingsRuntime/Logging/WUSettingsLog.h"

bool FWUVideoSettingApplier::CanApply(const FWUSettingDescriptor& Descriptor) const
{
	return Descriptor.Category == EWUSettingCategory::Video && Descriptor.ApplyStrategy == EWUSettingApplyStrategy::Immediate;
}

void FWUVideoSettingApplier::Apply(UWorld* World, const FWUSettingDescriptor& Descriptor, const FWUSettingValue& Value)
{
	if (!World)
	{
		return;
	}

	if (Descriptor.SettingName == TEXT("Video.VSync"))
	{
		if (Value.Type != EWUSettingType::Bool)
		{
			UE_LOG(LogWUSettingsVideo, Warning, TEXT("WUVideo: VSync setting must be bool"));
			return;
		}
		ApplyVSync(Value.BoolValue);
		return;
	}

	if (Descriptor.SettingName == TEXT("Video.FOV"))
	{
		if (Value.Type != EWUSettingType::Float)
		{
			UE_LOG(LogWUSettingsVideo, Warning, TEXT("WUVideo: FOV setting must be float"));
			return;
		}
		ApplyFOV(World, Value.FloatValue);
	}
}

void FWUVideoSettingApplier::ApplyVSync(bool bEnabled)
{
	UGameUserSettings* Settings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
	if (!Settings)
	{
		UE_LOG(LogWUSettingsVideo, Warning, TEXT("WUVideo: Failed to get GameUserSettings"));
		return;
	}

	// Toggle is already stored in GameUserSettings via cvar r.VSync
	Settings->SetVSyncEnabled(bEnabled);
	// Only non-resolution settings; resolution/window mode go through the ApplyConfirm flow.
	Settings->ApplyNonResolutionSettings();
	UE_LOG(LogWUSettingsVideo, Log, TEXT("WUVideo: Vsync set to %s"), bEnabled ? TEXT("Enabled") : TEXT("Disabled"));
}

void FWUVideoSettingApplier::ApplyFOV(UWorld* World, float FOV)
{
	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}
	APlayerCameraManager* PCM = PC->PlayerCameraManager;
	if (!PCM)
	{	
		return;
	}
	PCM->SetFOV(FOV);
	UE_LOG(LogWUSettingsVideo, Log, TEXT("FOV Applied"));
}
