// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsTypes.h"
#include "WUSettingsScreen.generated.h"

class UButton;
class UPanelWidget;
class UWUSettingRow;
class UWUSettingRowBool;
class UWUSettingRowEnum;
class UWUSettingRowResolution;
class UWUSettingRowSlider;

/**
 * Settings screen. Builds one row per registered setting of the current category, in registration order,
 * using the row classes set in Class Defaults. Category tabs are up to the Blueprint: call SetCategory.
 */
UCLASS(Abstract, meta=(DisableNativeTick))
class WUSETTINGSUI_API UWUSettingsScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="WUSettings")
	void SetCategory(EWUSettingCategory Category);

	UFUNCTION(BlueprintPure, Category="WUSettings")
	EWUSettingCategory GetCategory() const { return CurrentCategory; }

	// Categories that have at least one registered setting, in enum order (for building tabs).
	UFUNCTION(BlueprintPure, Category="WUSettings")
	TArray<EWUSettingCategory> GetAvailableCategories() const;

	UFUNCTION(BlueprintCallable, Category="WUSettings")
	void ResetCurrentCategory();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	// Lets the Blueprint highlight the active tab.
	UFUNCTION(BlueprintImplementableEvent, Category="WUSettings")
	void OnCategoryChanged(EWUSettingCategory NewCategory);

	// Any panel (VerticalBox, ScrollBox...). Its children are rebuilt on every category change.
	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidget))
	TObjectPtr<UPanelWidget> RowContainer;

	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidgetOptional))
	TObjectPtr<UButton> ResetButton;

	// Category shown first. Falls back to the first available one if it has no settings.
	UPROPERTY(EditAnywhere, Category="WUSettings")
	EWUSettingCategory InitialCategory = EWUSettingCategory::Video;

	UPROPERTY(EditDefaultsOnly, Category="WUSettings|Rows")
	TSubclassOf<UWUSettingRowBool> BoolRowClass;

	UPROPERTY(EditDefaultsOnly, Category="WUSettings|Rows")
	TSubclassOf<UWUSettingRowSlider> SliderRowClass;

	UPROPERTY(EditDefaultsOnly, Category="WUSettings|Rows")
	TSubclassOf<UWUSettingRowEnum> EnumRowClass;

	UPROPERTY(EditDefaultsOnly, Category="WUSettings|Rows")
	TSubclassOf<UWUSettingRowResolution> ResolutionRowClass;

private:
	UFUNCTION()
	void HandleResetClicked();

	TSubclassOf<UWUSettingRow> GetRowClass(EWUSettingType Type) const;
	void RebuildRows();

	EWUSettingCategory CurrentCategory = EWUSettingCategory::Video;
	bool bHasCategory = false;
};