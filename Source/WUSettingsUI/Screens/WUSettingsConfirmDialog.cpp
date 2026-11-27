// Copyright (c) 2026 Wanderson Lopes. All rights reserved.


#include "WUSettingsConfirmDialog.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "WUSettingsRuntime/Subsystems/WUSettingsSubsystem.h"

void UWUSettingsConfirmDialog::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	KeepButton->OnClicked.AddDynamic(this, &UWUSettingsConfirmDialog::HandleKeepClicked);
	RevertButton->OnClicked.AddDynamic(this, &UWUSettingsConfirmDialog::HandleRevertClicked);
}

void UWUSettingsConfirmDialog::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GameInstance = GetGameInstance();
	Subsystem = GameInstance ? GameInstance->GetSubsystem<UWUSettingsSubsystem>() : nullptr;

	if (Subsystem.IsValid())
	{
		Subsystem->OnConfirmRequired.AddUniqueDynamic(this, &UWUSettingsConfirmDialog::HandleConfirmRequired);
		Subsystem->OnPendingResolved.AddUniqueDynamic(this, &UWUSettingsConfirmDialog::HandlePendingResolved);
	}

	// Opened while a change is already pending (e.g. the menu was reopened): show it right away
	const bool bPending = Subsystem.IsValid() && Subsystem->HasPendingConfirmation();
	SetVisibility(bPending ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	UpdateCountdown();
}

void UWUSettingsConfirmDialog::NativeDestruct()
{
	if (Subsystem.IsValid())
	{
		Subsystem->OnConfirmRequired.RemoveDynamic(this, &UWUSettingsConfirmDialog::HandleConfirmRequired);
		Subsystem->OnPendingResolved.RemoveDynamic(this, &UWUSettingsConfirmDialog::HandlePendingResolved);
	}
	Subsystem.Reset();
	Super::NativeDestruct();
}

void UWUSettingsConfirmDialog::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateCountdown();
}

void UWUSettingsConfirmDialog::HandleKeepClicked()
{
	if (Subsystem.IsValid())
	{
		Subsystem->ConfirmPendingSettings();
	}
}

void UWUSettingsConfirmDialog::HandleRevertClicked()
{
	if (Subsystem.IsValid())
	{
		Subsystem->RevertPendingSettings();
	}
}

void UWUSettingsConfirmDialog::HandleConfirmRequired(float TimeoutSeconds)
{
	SetVisibility(ESlateVisibility::Visible);
	UpdateCountdown();
	// Gamepad/keyboard users land on Keep
	KeepButton->SetKeyboardFocus();
}

void UWUSettingsConfirmDialog::HandlePendingResolved(bool bConfirmed)
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWUSettingsConfirmDialog::UpdateCountdown() const
{
	if (CountdownText && Subsystem.IsValid())
	{
		const int32 Seconds = FMath::CeilToInt(Subsystem->GetConfirmTimeRemaining());
		CountdownText->SetText(FText::Format(CountdownFormat, FText::AsNumber(Seconds)));
	}
}
