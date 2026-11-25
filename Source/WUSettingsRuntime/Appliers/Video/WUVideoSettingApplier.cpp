// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUVideoSettingApplier.h"

#include "GameFramework/GameUserSettings.h"
#include "WUSettingsRuntime/Logging/WUSettingsLog.h"
#include "WUSettingsRuntime/Video/WUVideoResolutions.h"
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

	if (Descriptor.SettingName == TEXT("Video.Resolution"))
	{
		if (Value.Type != EWUSettingType::Resolution)
		{
			UE_LOG(LogWUSettingsVideo, Warning, TEXT("WUVideo: Resolution setting must be resolution"));
			return;
		}
		ApplyResolution(World, Value.ResolutionValue);
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

	Settings->SetFullscreenMode(ToEngineWindowMode(static_cast<EWUWindowMode>(ModeIndex)));
	ApplyDisplaySettings(Settings);
}

void FWUVideoSettingApplier::ApplyResolution(UWorld* World, const FIntPoint& Resolution)
{
	if (World->WorldType == EWorldType::PIE)
	{
		UE_LOG(LogWUSettingsVideo, Log, TEXT("WUVideo: Resolution skipped in PIE (test in Standalone Game)"));
		return;
	}

	UGameUserSettings* Settings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
	if (!Settings)
	{
		UE_LOG(LogWUSettingsVideo, Warning, TEXT("WUVideo: Failed to get GameUserSettings"));
		return;
	}

	// (0,0) = native: resolve now, so it follows the current monitor.
	const bool bNative = Resolution.X == 0 && Resolution.Y == 0;
	Settings->SetScreenResolution(bNative ? Settings->GetDesktopResolution() : Resolution);
	ApplyDisplaySettings(Settings);
}

void FWUVideoSettingApplier::ApplyDisplaySettings(UGameUserSettings* Settings)
{
	FIntPoint Resolution = Settings->GetScreenResolution();
	const EWindowMode::Type Mode = Settings->GetFullscreenMode();

	// Exclusive fullscreen only accepts modes the monitor supports; the engine silently ignores others.
	// Fall back to the desktop resolution. The saved value is kept, so it's used again in Windowed.
	if (Mode == EWindowMode::Fullscreen && !WUVideoResolutions::IsSupportedFullscreenResolution(Resolution))
	{
		const FIntPoint Desktop = Settings->GetDesktopResolution();
		UE_LOG(LogWUSettingsVideo, Warning, TEXT("WUVideo: %dx%d is not a supported fullscreen mode; using desktop resolution %dx%d"),
			Resolution.X, Resolution.Y, Desktop.X, Desktop.Y);

		Resolution = Desktop;
		Settings->SetScreenResolution(Resolution);
	}

	// Compare against the *actual* display state, not the saved one:
	// command-line overrides (e.g. -windowed -ResX/-ResY from the editor) make them differ.
	if (GSystemResolution.WindowMode == Mode &&
		static_cast<int32>(GSystemResolution.ResX) == Resolution.X &&
		static_cast<int32>(GSystemResolution.ResY) == Resolution.Y)
	{
		return;
	}

	Settings->ApplyResolutionSettings(false);

	// Mirror to GameUserSettings.ini so the engine boots straight into this state (no switch at startup).
	// When the Apply/Confirm flow exists (Phase 7), ConfirmVideoMode moves to the confirm step.
	Settings->ConfirmVideoMode();
	Settings->SaveSettings();

	UE_LOG(LogWUSettingsVideo, Log, TEXT("WUVideo: Display set to %dx%d, mode %s"), Resolution.X, Resolution.Y, LexToString(Mode));
}
