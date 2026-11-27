// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WUSettingRow.h"
#include "WUSettingRowSlider.generated.h"

class USlider;
class UTextBlock;

/**
 * Float or Int setting shown as a slider, using the descriptor's Min/Max/Step.
 * While dragging only the value text updates; the value is committed (and saved) on release.
 * Keyboard/gamepad steps commit immediately.
 */
UCLASS(Abstract, meta=(DisableNativeTick))
class WUSETTINGSUI_API UWUSettingRowSlider : public UWUSettingRow
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual bool SupportsType(EWUSettingType Type) const override;
	virtual void InitControl() override;
	virtual void RefreshFromValue() override;

	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidget))
	TObjectPtr<USlider> Slider;

	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ValueText;

private:
	UFUNCTION()
	void HandleValueChanged(float NewValue);

	UFUNCTION()
	void HandleCaptureBegin();

	UFUNCTION()
	void HandleCaptureEnd();

	float GetEffectiveStep() const;
	float SnapToStep(float RawValue) const;
	void UpdateValueText(float Value) const;
	void CommitSliderValue(float RawValue);

	bool bDragging = false;
};
