// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/SlateEnums.h"
#include "WUSettingRow.h"
#include "WUSettingsRuntime/Video/WUVideoTypes.h"
#include "WUSettingRowResolution.generated.h"

class UComboBoxString;

/**
 * Resolution setting shown as a combo box. Options follow the current window mode:
 * Borderless -> native only (disabled), Fullscreen -> supported modes, Windowed -> sizes that fit the desktop.
 */
UCLASS(Abstract, meta=(DisableNativeTick))
class WUSETTINGSUI_API UWUSettingRowResolution : public UWUSettingRow
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual bool SupportsType(EWUSettingType Type) const override { return Type == EWUSettingType::Resolution; }
	virtual void InitControl() override;
	virtual void RefreshFromValue() override;
	virtual void OnOtherSettingChanged(FName ChangedSetting) override;

	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidget))
	TObjectPtr<UComboBoxString> ComboBox;

	// The window mode setting whose value decides which resolutions are offered.
	UPROPERTY(EditAnywhere, Category = "WUSettings")
	FName WindowModeSettingName = TEXT("Video.WindowMode");

	// Label of the native option; {0} is the desktop resolution.
	UPROPERTY(EditAnywhere, Category = "WUSettings")
	FText NativeLabelFormat = NSLOCTEXT("WUSettings", "NativeResolution", "Native ({0})");

private:
	UFUNCTION()
	void HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	EWUWindowMode GetWindowMode() const;
	static FString FormatResolution(const FIntPoint& Resolution);

	// Parallel to the combo box options. (0,0) = native.
	TArray<FIntPoint> Options;
};
