// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUSettingRowEnum.h"

#include "Components/ComboBoxString.h"

void UWUSettingRowEnum::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ComboBox->OnSelectionChanged.AddDynamic(this, &UWUSettingRowEnum::HandleSelectionChanged);
}

void UWUSettingRowEnum::InitControl()
{
	ComboBox->ClearOptions();

	const int32 Min = FMath::RoundToInt(Descriptor.MinValue);
	const int32 Max = FMath::RoundToInt(Descriptor.MaxValue);
	for (int32 EnumValue = Min; EnumValue <= Max; ++EnumValue)
	{
		ComboBox->AddOption(Descriptor.EnumOptions.IsValidIndex(EnumValue)
			? Descriptor.EnumOptions[EnumValue].ToString()
			: FString::FromInt(EnumValue));
	}
}

void UWUSettingRowEnum::RefreshFromValue()
{
	FWUSettingValue Value;
	if (GetCurrentValue(Value))
	{
		ComboBox->SetSelectedIndex(Value.EnumValue - FMath::RoundToInt(Descriptor.MinValue));
	}
}

void UWUSettingRowEnum::HandleSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// SetSelectedIndex (our own refresh) reports Direct; only user picks are committed
	if (SelectionType == ESelectInfo::Direct)
	{
		return;
	}

	const int32 Index = ComboBox->GetSelectedIndex();
	if (Index >= 0)
	{
		CommitValue(FWUSettingValue::MakeEnum(FMath::RoundToInt(Descriptor.MinValue) + Index));
	}
}