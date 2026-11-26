// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUSettingsSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "WUSettingsRuntime/Appliers/WUSettingApplier.h"
#include "WUSettingsRuntime/Appliers/WUSettingsApplierRegistry.h"
#include "WUSettingsRuntime/Logging/WUSettingsLog.h"
#include "WUSettingsRuntime/Settings/WUSettingsDeveloperSettings.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsRegistry.h"


void UWUSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadSettings();
	// No world exists yet, UWUSettingsWorldSubsystem applies everything on each world BeginPlay.
}

void UWUSettingsSubsystem::Deinitialize()
{
	// Unconfirmed values were never saved, so quitting while pending effectively reverts them.
	StopConfirmTimer();

	Super::Deinitialize();
}

bool UWUSettingsSubsystem::GetSettingValue(FName SettingName, FWUSettingValue& OutValue) const
{
	if (const FWUSettingValue* Cached = CachedValues.Find(SettingName))
	{
		OutValue = *Cached;
		return true;
	}

	return FWUSettingsRegistry::Get().GetDefaultValue(SettingName, OutValue);
}

bool UWUSettingsSubsystem::SetSettingValue(FName SettingName, const FWUSettingValue& Value)
{
	const FWUSettingDescriptor* Descriptor = FWUSettingsRegistry::Get().FindSetting(SettingName);
	if (!Descriptor)
	{
		UE_LOG(LogWUSettings, Warning, TEXT("SetSettingValue: unknown setting '%s'"), *SettingName.ToString());
		return false;
	}

	FWUSettingValue Sanitized;
	if (!SanitizeValue(*Descriptor, Value, Sanitized))
	{
		UE_LOG(LogWUSettings, Warning, TEXT("SetSettingValue: invalid value for '%s' (expected %s, got %s)"), *SettingName.ToString(), *UEnum::GetValueAsString(Descriptor->Type), *UEnum::GetValueAsString(Value.Type));
		return false;
	}

	UWorld* World = GetWorld();

	// Without a world nothing gets applied, so there is nothing for the player to confirm: just save
	const bool bNeedsConfirm = World && Descriptor->ApplyStrategy == EWUSettingApplyStrategy::ApplyConfirm;
	if (bNeedsConfirm && !PendingRollback.Contains(SettingName))
	{
		// Keep the value from before the *first* unconfirmed change, so a revert goes all the way back.
		FWUSettingValue Previous;
		GetSettingValue(SettingName, Previous);
		PendingRollback.Add(SettingName, Previous);
	}

	CachedValues.Add(SettingName, Sanitized);

	if (World)
	{
		ApplySetting(World, *Descriptor, Sanitized);
	}

	OnSettingChanged.Broadcast(SettingName);

	if (bNeedsConfirm)
	{
		StartConfirmTimer();
		return true;
	}

	// Always save, even without a world
	SaveSettings();
	return true;
}

void UWUSettingsSubsystem::SaveSettings()
{
	if (!SaveGameObject)
	{
		UE_LOG(LogWUSettings, Error, TEXT("SaveSettings: no SaveGame object; settings not saved"));
		return;
	}

	// Never persist unconfirmed values: write their rollback value instead.
	TMap<FName, FWUSettingValue> ToSave = CachedValues;
	for (const TPair<FName, FWUSettingValue>& Pair : PendingRollback)
	{
		ToSave.Add(Pair.Key, Pair.Value);
	}

	SaveGameObject->Version = UWUSettingSaveGame::CurrentVersion;
	SaveGameObject->StoredSettings = MoveTemp(ToSave);

	if (!UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveSlot, UserIndex))
	{
		UE_LOG(LogWUSettings, Error, TEXT("Failed to save settings to slot '%s'"), SaveSlot);
	}
}

void UWUSettingsSubsystem::ApplyAllRegisteredSettings(UWorld* World)
{
	if (!World)
	{
		return;
	}

	for (const FWUSettingDescriptor& Descriptor : FWUSettingsRegistry::Get().GetAllSettings())
	{
		FWUSettingValue Value;
		if (GetSettingValue(Descriptor.SettingName, Value))
		{
			ApplySetting(World, Descriptor, Value);
		}
	}

	UE_LOG(LogWUSettings, Log, TEXT("Applied all settings to world '%s'"), *World->GetName());
}

void UWUSettingsSubsystem::ConfirmPendingSettings()
{
	if (PendingRollback.IsEmpty())
	{
		return;
	}

	StopConfirmTimer();
	const int32 Count = PendingRollback.Num();
	PendingRollback.Reset();
	SaveSettings();

	UE_LOG(LogWUSettings, Log, TEXT("Confirmed %d pending setting(s)"), Count);
	OnPendingResolved.Broadcast(true);
}

void UWUSettingsSubsystem::RevertPendingSettings()
{
	if (PendingRollback.IsEmpty())
	{
		return;
	}

	StopConfirmTimer();

	// Clear the pending state before applying, so anything reacting to the apply sees no pending settings.
	TMap<FName, FWUSettingValue> Rollback = MoveTemp(PendingRollback);
	PendingRollback.Reset();

	UWorld* World = GetWorld();
	const FWUSettingsRegistry& Registry = FWUSettingsRegistry::Get();
	for (const TPair<FName, FWUSettingValue>& Pair : Rollback)
	{
		CachedValues.Add(Pair.Key, Pair.Value);

		const FWUSettingDescriptor* Descriptor = Registry.FindSetting(Pair.Key);
		if (World && Descriptor)
		{
			ApplySetting(World, *Descriptor, Pair.Value);
		}
		OnSettingChanged.Broadcast(Pair.Key);
	}

	// The .sav never contained the pending values, so there is nothing to save.
	UE_LOG(LogWUSettings, Log, TEXT("Reverted %d pending setting(s)"), Rollback.Num());
	OnPendingResolved.Broadcast(false);
}

float UWUSettingsSubsystem::GetConfirmTimeRemaining() const
{
	if (PendingRollback.IsEmpty())
	{
		return 0.f;
	}
	return static_cast<float>(FMath::Max(0.0, ConfirmDeadline - FPlatformTime::Seconds()));
}

void UWUSettingsSubsystem::StartConfirmTimer()
{
	// Every new unconfirmed change restarts the countdown
	StopConfirmTimer();

	const float Timeout = GetDefault<UWUSettingsDeveloperSettings>()->ConfirmTimeoutSeconds;
	ConfirmDeadline = FPlatformTime::Seconds() + Timeout;
	ConfirmTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &UWUSettingsSubsystem::HandleConfirmTimeout), Timeout);

	UE_LOG(LogWUSettings, Log, TEXT("%d setting(s) pending confirmation; reverting in %.0fs"), PendingRollback.Num(), Timeout);
	OnConfirmRequired.Broadcast(Timeout);
}

void UWUSettingsSubsystem::StopConfirmTimer()
{
	if (ConfirmTickerHandle.IsValid())
	{
		FTSTicker::RemoveTicker(ConfirmTickerHandle);
		ConfirmTickerHandle.Reset();
	}
}

bool UWUSettingsSubsystem::HandleConfirmTimeout(float DeltaTime)
{
	// The ticker removes itself when we return false; drop the handle so Revert doesn't remove it again
	ConfirmTickerHandle.Reset();

	UE_LOG(LogWUSettings, Log, TEXT("Confirmation timed out"));
	RevertPendingSettings();
	return false;
}

void UWUSettingsSubsystem::LoadSettings()
{
	SaveGameObject = nullptr;
	CachedValues.Reset();

	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, UserIndex))
	{
		SaveGameObject = Cast<UWUSettingSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, UserIndex));

		if (!SaveGameObject)
		{
			UE_LOG(LogWUSettings, Warning, TEXT("Save slot '%s' could not be loaded; using defaults"), SaveSlot);
		}
		else if (SaveGameObject->Version > UWUSettingSaveGame::CurrentVersion)
		{
			UE_LOG(LogWUSettings, Warning, TEXT("Save version %d is newer than supported %d; using defaults"), SaveGameObject->Version, UWUSettingSaveGame::CurrentVersion);
			SaveGameObject = nullptr;
		}
	}

	if (!SaveGameObject)
	{
		SaveGameObject = Cast<UWUSettingSaveGame>(UGameplayStatics::CreateSaveGameObject(UWUSettingSaveGame::StaticClass()));
		return;
	}

	// Future migrations (Version < CurrentVersion) go here.

	const FWUSettingsRegistry& Registry = FWUSettingsRegistry::Get();
	for (const TPair<FName, FWUSettingValue>& Pair : SaveGameObject->StoredSettings)
	{
		const FWUSettingDescriptor* Descriptor = Registry.FindSetting(Pair.Key);
		if (!Descriptor)
		{
			UE_LOG(LogWUSettings, Log, TEXT("Dropping unknown saved setting '%s'"), *Pair.Key.ToString());
			continue;
		}

		FWUSettingValue Sanitized;
		if (!SanitizeValue(*Descriptor, Pair.Value, Sanitized))
		{
			UE_LOG(LogWUSettings, Warning, TEXT("Dropping saved setting '%s': type no longer matches"), *Pair.Key.ToString());
			continue;
		}
		CachedValues.Add(Pair.Key, Sanitized);
	}

	UE_LOG(LogWUSettings, Log, TEXT("Loaded %d saved settings."), CachedValues.Num());
}

void UWUSettingsSubsystem::ApplySetting(UWorld* World, const FWUSettingDescriptor& Descriptor,
	const FWUSettingValue& Value) const
{
	if (IWUSettingApplier* Applier = FWUSettingsApplierRegistry::Get().FindApplier(Descriptor))
	{
		Applier->Apply(World, Descriptor, Value);
	}
}

bool UWUSettingsSubsystem::SanitizeValue(const FWUSettingDescriptor& Descriptor, const FWUSettingValue& InValue,
	FWUSettingValue& OutValue)
{
	if (InValue.Type != Descriptor.Type)
	{
		return false;
	}

	OutValue = InValue;

	switch (Descriptor.Type)
	{
	case EWUSettingType::Float:
		if (!FMath::IsFinite(InValue.FloatValue))
		{
			return false;
		}
		OutValue.FloatValue = FMath::Clamp(InValue.FloatValue, Descriptor.MinValue, Descriptor.MaxValue);
		break;

	case EWUSettingType::Int:
		OutValue.IntValue = FMath::Clamp(InValue.IntValue, FMath::RoundToInt(Descriptor.MinValue), FMath::RoundToInt(Descriptor.MaxValue));
		break;
	case EWUSettingType::Enum:
		// Enum descriptors must set MinValue/MaxValue to the valid index range.
		// Out-of-range is rejected (not clamped): clamping an enum picks an arbitrary option.
		if (InValue.EnumValue < FMath::RoundToInt(Descriptor.MinValue) || InValue.EnumValue > FMath::RoundToInt(Descriptor.MaxValue))
		{
			return false;
		}
		break;

	case EWUSettingType::Resolution:
		{
			// (0,0) = native. Otherwise reject nonsense sizes; whether the monitor supports it is the applier's call.
			const FIntPoint& Res = InValue.ResolutionValue;
			const bool bNative = Res.X == 0 && Res.Y == 0;
			if (!bNative && (Res.X < 640 || Res.Y < 360))
			{
				return false;
			}
		}
		break;

	default:
		// Bool: nothing to validate here.
		break;
	}
	return true;
}
