// Copyright (c) 2026 Wanderson Lopes. All rights reserved.


#include "WUSettingRowBool.h"

#include "Components/CheckBox.h"

void UWUSettingRowBool::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// SetIsChecked doesn't fire this, so refreshing from code never loops back into a commit
	CheckBox->OnCheckStateChanged.AddDynamic(this, &UWUSettingRowBool::HandleCheckStateChanged);
}

void UWUSettingRowBool::RefreshFromValue()
{
	FWUSettingValue Value;
	if (GetCurrentValue(Value))
	{
		CheckBox->SetIsChecked(Value.BoolValue);
	}
}

void UWUSettingRowBool::HandleCheckStateChanged(bool bIsChecked)
{
	CommitValue(FWUSettingValue::MakeBool(bIsChecked));
}
