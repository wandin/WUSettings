// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUVideoSettingApplier.h"

#include "GameFramework/GameUserSettings.h"
#include "WUSettingsRuntime/Logging/WUSettingsLog.h"
#include "WUSettingsRuntime/Video/WUVideoTypes.h"
#include "UnrealEngine.h"

namespace
{
	EWindowMode::Type ToEngineWindowMode(EWUWindowMode Mode)
	{
		switch (Mode)
		{
		case EWUWindowMode::Windowed: return EWindowMode::Windowed;
		case EWUWindowMode::Fullscreen: return EWindowMode::Fullscreen;
		case EWUWindowMode::Borderless:
		default: return EWindowMode::WindowedFullscreen;
		}
	}
}

bool FWUVideoSettingApplier::CanApply(const FWUSettingDescriptor& Descriptor) const
{
	// ApplyStrategy decides *when* to apply (Subsystem's job); the applier only knows *how*
	return Descriptor.Category == EWUSettingCategory::Video;
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
		return;
	}

	if (Descriptor.SettingName == TEXT("Video.WindowMode"))
	{
		if (Value.Type != EWUSettingType::Enum)
		{
			UE_LOG(LogWUSettingsVideo, Warning, TEXT("WUVideo: WindowMode setting must be enum"));
			return;
		}
		ApplyWindowMode(World, Value.EnumValue);
		return;
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

void FWUVideoSettingApplier::ApplyWindowMode(UWorld* World, int32 ModeIndex)
{
	// PIE runs inside the editor window; switching window mode there would affect the editor itself
	if (World->WorldType == EWorldType::PIE)
	{
		UE_LOG(LogWUSettingsVideo, Log, TEXT("WUVideo: WindowMode skipped in PIE (test in Standalone Game)"));
		return;
	}

	UGameUserSettings* Settings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
	if (!Settings)
	{
		UE_LOG(LogWUSettingsVideo, Warning, TEXT("WUVideo: Failed to get GameUserSettings"));
		return;
	}

	const EWUWindowMode Mode = static_cast<EWUWindowMode>(ModeIndex);
	const EWindowMode::Type Target = ToEngineWindowMode(Mode);

	// Compare against the *actual* window state, not the saved GameUserSettings value:
	// command-line overrides (e.g. -windowed from the editor's Standalone launch) make them differ.
	if (GSystemResolution.WindowMode == Target)
	{
		return;
	}

	Settings->SetFullscreenMode(Target);
	Settings->ApplyResolutionSettings(false);

	// Mirror to GameUserSettings.ini so the engine boots straight into this mode (no switch at startup).
	// When the Apply/Confirm flow exists (Phase 7), ConfirmVideoMode moves to the confirm step.
	Settings->ConfirmVideoMode();
	Settings->SaveSettings();

	UE_LOG(LogWUSettingsVideo, Log, TEXT("WUVideo: WindowMode set to %s"), *UEnum::GetValueAsString(Mode));
}
