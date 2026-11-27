// Copyright (c) 2026 Wanderson Lopes. All rights reserved.


#include "WUSettingRow.h"

#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsRegistry.h"
#include "WUSettingsRuntime/Subsystems/WUSettingsSubsystem.h"
#include "WUSettingsUI/Logging/WUSettingsUILog.h"

void UWUSettingRow::SetSettingName(FName InSettingName)
{
	SettingName = InSettingName;
	if (bConstructed)
	{
		Bind();
	}
}

void UWUSettingRow::NativeConstruct()
{
	Super::NativeConstruct();
	bConstructed = true;
	Bind();
}

void UWUSettingRow::NativeDestruct()
{
	Unbind();
	bConstructed = false;
	Super::NativeDestruct();
}

bool UWUSettingRow::GetCurrentValue(FWUSettingValue& OutValue) const
{
	return bHasDescriptor && Subsystem.IsValid() && Subsystem->GetSettingValue(SettingName, OutValue);
}

UWUSettingsSubsystem* UWUSettingRow::GetSettingsSubsystem() const
{
	return Subsystem.Get();
}

void UWUSettingRow::CommitValue(const FWUSettingValue& Value)
{
	if (!bHasDescriptor || !Subsystem.IsValid())
	{
		return;
	}

	UE_LOG(LogWUSettingsUI, Verbose, TEXT("Row '%s': commit"), *SettingName.ToString());

	if (!Subsystem->SetSettingValue(SettingName, Value))
	{
		RefreshFromValue();
	}
}

void UWUSettingRow::Bind()
{
	Unbind();
	
	if (SettingName.IsNone())
	{
		return;
	}

	const FWUSettingDescriptor* Found = FWUSettingsRegistry::Get().FindSetting(SettingName);
	UGameInstance* GameInstance = GetGameInstance();
	UWUSettingsSubsystem* Settings = GameInstance ? GameInstance->GetSubsystem<UWUSettingsSubsystem>() : nullptr;

	if (!Found || !Settings)
	{
		UE_LOG(LogWUSettingsUI, Warning, TEXT("Row '%s': unknown setting or no settings subsystem"), *SettingName.ToString());
		return;
	}

	if (!SupportsType(Found->Type))
	{
		UE_LOG(LogWUSettingsUI, Warning, TEXT("Row '%s': %s can't display type %s"), *SettingName.ToString(), *GetClass()->GetName(), *UEnum::GetValueAsString(Found->Type));
		return;
	}

	Descriptor = *Found;
	Subsystem = Settings;
	bHasDescriptor = true;
	SettingChangedHandle = Settings->OnSettingChanged.AddUObject(this, &UWUSettingRow::HandleSettingChanged);

	if (LabelText)
	{
		LabelText->SetText(Descriptor.DisplayName);
	}
	SetToolTipText(Descriptor.Description);

	InitControl();
	RefreshFromValue();
}

void UWUSettingRow::Unbind()
{
	if (Subsystem.IsValid())
	{
		Subsystem->OnSettingChanged.Remove(SettingChangedHandle);
	}
	SettingChangedHandle.Reset();
	Subsystem.Reset();
	bHasDescriptor = false;
}

void UWUSettingRow::HandleSettingChanged(FName ChangedSetting)
{
	if (ChangedSetting == SettingName)
	{
		RefreshFromValue();
	}
	else
	{
		OnOtherSettingChanged(ChangedSetting);
	}
}
