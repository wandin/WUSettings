// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WUSettingsConfirmDialog.generated.h"

class UWUSettingsSubsystem;
class UTextBlock;
class UButton;
/**
 * "Keep these settings?" dialog for ApplyConfirm settings. Self-managed: shows itself on
 * OnConfirmRequired, hides on OnPendingResolved, and counts down until the automatic revert.
 * Ticks natively for the countdown (no DisableNativeTick); collapsed widgets don't tick.
 */
UCLASS(Abstract)
class WUSETTINGSUI_API UWUSettingsConfirmDialog : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidget))
	TObjectPtr<UButton> KeepButton;

	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidget))
	TObjectPtr<UButton> RevertButton;

	UPROPERTY(BlueprintReadOnly, Category="WUSettings", meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> CountdownText;

	// {0} is the number of seconds left.
	UPROPERTY(EditAnywhere, Category="WUSettings")
	FText CountdownFormat = NSLOCTEXT("WUSettings", "ConfirmCountdown", "Reverting in {0}s");

private:
	UFUNCTION()
	void HandleKeepClicked();

	UFUNCTION()
	void HandleRevertClicked();

	UFUNCTION()
	void HandleConfirmRequired(float TimeoutSeconds);

	UFUNCTION()
	void HandlePendingResolved(bool bConfirmed);

	void UpdateCountdown() const;

	TWeakObjectPtr<UWUSettingsSubsystem> Subsystem;
};
