// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUSettingsScreen.h"

#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Engine/GameInstance.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsRegistry.h"
#include "WUSettingsRuntime/Subsystems/WUSettingsSubsystem.h"
#include "WUSettingsUI/Logging/WUSettingsUILog.h"
#include "WUSettingsUI/Rows/WUSettingRowBool.h"
#include "WUSettingsUI/Rows/WUSettingRowEnum.h"
#include "WUSettingsUI/Rows/WUSettingRowResolution.h"
#include "WUSettingsUI/Rows/WUSettingRowSlider.h"

void UWUSettingsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ResetButton)
	{
		ResetButton->OnClicked.AddDynamic(this, &UWUSettingsScreen::HandleResetClicked);
	}
}

void UWUSettingsScreen::NativeConstruct()
{
	Super::NativeConstruct();

	if (bHasCategory)
	{
		// Re-added to the viewport: keep the category the player was on
		RebuildRows();
		return;
	}

	const TArray<EWUSettingCategory> Available = GetAvailableCategories();
	if (Available.Contains(InitialCategory))
	{
		SetCategory(InitialCategory);
	}
	else if (!Available.IsEmpty())
	{
		SetCategory(Available[0]);
	}
}

void UWUSettingsScreen::SetCategory(EWUSettingCategory Category)
{
	CurrentCategory = Category;
	bHasCategory = true;
	RebuildRows();
	OnCategoryChanged(Category);
}

TArray<EWUSettingCategory> UWUSettingsScreen::GetAvailableCategories() const
{
	TArray<EWUSettingCategory> Categories;
	for (const FWUSettingDescriptor& Descriptor : FWUSettingsRegistry::Get().GetAllSettings())
	{
		Categories.AddUnique(Descriptor.Category);
	}
	Categories.Sort();
	return Categories;
}

void UWUSettingsScreen::ResetCurrentCategory()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (UWUSettingsSubsystem* Settings = GameInstance ? GameInstance->GetSubsystem<UWUSettingsSubsystem>() : nullptr)
	{
		Settings->ResetCategoryToDefaults(CurrentCategory);
	}
}

void UWUSettingsScreen::HandleResetClicked()
{
	ResetCurrentCategory();
}

TSubclassOf<UWUSettingRow> UWUSettingsScreen::GetRowClass(EWUSettingType Type) const
{
	switch (Type)
	{
	case EWUSettingType::Bool:       return BoolRowClass;
	case EWUSettingType::Int:
	case EWUSettingType::Float:      return SliderRowClass;
	case EWUSettingType::Enum:       return EnumRowClass;
	case EWUSettingType::Resolution: return ResolutionRowClass;
	default:                         return nullptr;
	}
}

void UWUSettingsScreen::RebuildRows()
{
	RowContainer->ClearChildren();

	for (const FWUSettingDescriptor& Descriptor : FWUSettingsRegistry::Get().GetAllSettings())
	{
		if (Descriptor.Category != CurrentCategory)
		{
			continue;
		}

		const TSubclassOf<UWUSettingRow> RowClass = GetRowClass(Descriptor.Type);
		if (!RowClass)
		{
			UE_LOG(LogWUSettingsUI, Warning, TEXT("Screen: no row class set for type %s (setting '%s')"),
				*UEnum::GetValueAsString(Descriptor.Type), *Descriptor.SettingName.ToString());
			continue;
		}

		// The name is set before the row is added, so it binds once when it gets constructed
		UWUSettingRow* Row = CreateWidget<UWUSettingRow>(this, RowClass);
		Row->SetSettingName(Descriptor.SettingName);
		RowContainer->AddChild(Row);
	}
}