// Copyright (c) 2026 Wanderson Lopes. All rights reserved.


#include "WUSettingRowResolution.h"

#include "Components/ComboBoxString.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "WUSettingsRuntime/Subsystems/WUSettingsSubsystem.h"
#include "WUSettingsRuntime/Video/WUVideoResolutions.h"

void UWUSettingRowResolution::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ComboBox->OnSelectionChanged.AddDynamic(this, &UWUSettingRowResolution::HandleSelectionChanged);
}

void UWUSettingRowResolution::InitControl()
{
	Options.Reset();
	ComboBox->ClearOptions();

	const UGameUserSettings* GameSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
	const FIntPoint Desktop = GameSettings ? GameSettings->GetDesktopResolution() : FIntPoint::ZeroValue;

	Options.Add(FIntPoint::ZeroValue);
	ComboBox->AddOption(FText::Format(NativeLabelFormat, FText::FromString(FormatResolution(Desktop))).ToString());

	const EWUWindowMode Mode = GetWindowMode();

	// Borderless always covers the desktop at native resolution, so there is nothing to choose
	ComboBox->SetIsEnabled(Mode != EWUWindowMode::Borderless);
	if (Mode == EWUWindowMode::Borderless)
	{
		return;
	}

	const TArray<FIntPoint> Resolutions = Mode == EWUWindowMode::Fullscreen ?
	WUVideoResolutions::GetSupportedFullscreenResolutions() : WUVideoResolutions::GetWindowedResolutions();

	for (const FIntPoint& Resolution : Resolutions)
	{
		Options.Add(Resolution);
		ComboBox->AddOption(FormatResolution(Resolution));
	}
}

void UWUSettingRowResolution::RefreshFromValue()
{
	FWUSettingValue Value;
	if (!GetCurrentValue(Value))
	{
		return;
	}

	if (GetWindowMode() == EWUWindowMode::Borderless)
	{
		ComboBox->SetSelectedIndex(0);
		return;
	}

	int32 Index = Options.IndexOfByKey(Value.ResolutionValue);
	if (Index == INDEX_NONE)
	{
		// Not in the list (e.g. set from the console): show it anyway so the UI reflects the saved value
		Index = Options.Add(Value.ResolutionValue);
		ComboBox->AddOption(FormatResolution(Value.ResolutionValue));
	}
	ComboBox->SetSelectedIndex(Index);
}

void UWUSettingRowResolution::OnOtherSettingChanged(FName ChangedSetting)
{
	if (ChangedSetting == WindowModeSettingName)
	{
		InitControl();
		RefreshFromValue();
	}
}

void UWUSettingRowResolution::HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// SetSelectedIndex (our own refresh) reports Direct; only user picks are committed
	if (SelectionType == ESelectInfo::Direct)
	{
		return;
	}

	const int32 Index = ComboBox->GetSelectedIndex();
	if (Options.IsValidIndex(Index))
	{
		CommitValue(FWUSettingValue::MakeResolution(Options[Index].X, Options[Index].Y));
	}
}

EWUWindowMode UWUSettingRowResolution::GetWindowMode() const
{
	FWUSettingValue Value;
	const UWUSettingsSubsystem* Settings = GetSettingsSubsystem();
	if (Settings && Settings->GetSettingValue(WindowModeSettingName, Value) && Value.Type == EWUSettingType::Enum)
	{
		return static_cast<EWUWindowMode>(Value.EnumValue);
	}
	return EWUWindowMode::Borderless;
}

FString UWUSettingRowResolution::FormatResolution(const FIntPoint& Resolution)
{
	return FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y);
}
