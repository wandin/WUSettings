// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/SlateEnums.h"
#include "WUSettingRow.h"
#include "WUSettingRowEnum.generated.h"

class UComboBoxString;

// Enum setting shown as a combo box, labeled with the descriptor's EnumOptions.
UCLASS(Abstract)
class WUSETTINGSUI_API UWUSettingRowEnum : public UWUSettingRow
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual bool SupportsType(EWUSettingType Type) const override { return Type == EWUSettingType::Enum; }
	virtual void InitControl() override;
	virtual void RefreshFromValue() override;

	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidget))
	TObjectPtr<UComboBoxString> ComboBox;

private:
	UFUNCTION()
	void HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
};