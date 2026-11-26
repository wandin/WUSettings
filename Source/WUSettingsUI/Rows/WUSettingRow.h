// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WUSettingsRuntime/Persistence/WUSettingValue.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingDescriptor.h"
#include "WUSettingRow.generated.h"

class UTextBlock;
class UWUSettingsSubsystem;
/**
 * Base widget for one setting. Binds to SettingName, shows its label and keeps the control in sync
 * with UWUSettingsSubsystem (including external changes such as console commands or Apply/Confirm reverts).
 * Subclasses own the control; Blueprint children only provide layout and styling.
 */
UCLASS(Abstract)
class WUSETTINGSUI_API UWUSettingRow : public UUserWidget
{
	GENERATED_BODY()

public:
	// Binds the row to another setting. Rows placed in the designer bind themselves on construct
	UFUNCTION(BlueprintCallable, Category = "WUSettings")
	void SetSettingName(FName InSettingName);

	FName GetSettingName() const { return SettingName; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Which setting types this row can display.
	virtual bool SupportsType(EWUSettingType Type) const { return false; }

	// Called once the descriptor is known: configure the control (range, options).
	virtual void InitControl() {}

	// Show the current value in the control.
	virtual void RefreshFromValue() {}

	// Another setting changed (e.g the Resolution row depends on WindowMode).
	virtual void OnOtherSettingChanged(FName ChangedSetting) {}

	bool HasDescriptor() const { return bHasDescriptor; }
	bool GetCurrentValue(FWUSettingValue& OutValue) const;

	// Sends the value to the Subsystem. On success the Subsystem's OnSettingChanged refreshes the row
	// with the sanitized value; on rejection the row refreshes itself to undo the control change.
	void CommitValue(const FWUSettingValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WUSettings", meta=(ExposeOnSpawn))
	FName SettingName;

	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> LabelText;

	// Copy, not a pointer: the Registry's array may reallocate when settings are registered.
	FWUSettingDescriptor Descriptor;
	
private:

	void Bind();
	void Unbind();
	void HandleSettingChanged(FName ChangedSetting);

	TWeakObjectPtr<UWUSettingsSubsystem> Subsystem;
	FDelegateHandle SettingChangedHandle;
	bool bHasDescriptor = false;
	bool bConstructed = false;
};
