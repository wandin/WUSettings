// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WUSettingRow.h"
#include "WUSettingRowBool.generated.h"

class UCheckBox;
/**
 * Bool setting shown as a checkbox.
 */
UCLASS(Abstract, meta=(DisableNativeTick))
class WUSETTINGSUI_API UWUSettingRowBool : public UWUSettingRow
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual bool SupportsType(EWUSettingType Type) const override { return Type == EWUSettingType::Bool; }
	virtual void RefreshFromValue() override;

	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidget))
	TObjectPtr<UCheckBox> CheckBox;

private:
	UFUNCTION()
	void HandleCheckStateChanged(bool bIsChecked);
};
