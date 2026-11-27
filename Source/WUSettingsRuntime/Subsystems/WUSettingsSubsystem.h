// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WUSettingsRuntime/Persistence/WUSettingSaveGame.h"
#include "WUSettingsRuntime/Persistence/WUSettingValue.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingDescriptor.h"
#include "WUSettingsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWUOnConfirmRequired, float, TimeoutSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWUOnPendingResolved, bool, bConfirmed);
// Native C++ event: SettingName whose value changed (set, or reverted).
DECLARE_MULTICAST_DELEGATE_OneParam(FWUOnSettingChanged, FName);

UCLASS()
class WUSETTINGSRUNTIME_API UWUSettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Returns the current value (pending included), or the descriptor default. False if the setting is unknown.
	bool GetSettingValue(FName SettingName, FWUSettingValue& OutValue) const;

	// Validates (known name, matching type), clamps to Min/Max and applies.
	// Immediate settings are saved right away; ApplyConfirm settings wait for ConfirmPendingSettings
	// and revert on timeout. Returns false if the value was rejected.
	bool SetSettingValue(FName SettingName, const FWUSettingValue& Value);

	// Saves confirmed values. Pending (unconfirmed) values are written as their rollback value.
	void SaveSettings();

	// Called by UWUSettingsWorldSubsystem when a game world begins play.
	void ApplyAllRegisteredSettings(UWorld* World);

	// Apply/Confirm flow
	UFUNCTION(BlueprintCallable, Category = "WUSettings")
	void ConfirmPendingSettings();

	UFUNCTION(BlueprintCallable, Category = "WUSettings")
	void RevertPendingSettings();

	UFUNCTION(BlueprintPure, Category = "WUSettings")
	bool HasPendingConfirmation() const { return !PendingRollback.IsEmpty(); }

	UFUNCTION(BlueprintPure, Category = "WUSettings")
	float GetConfirmTimeRemaining() const;

	// Sets every setting of Category back to its default. Only values that differ are changed,
	// so unchanged ApplyConfirm settings don't trigger a confirmation.
	UFUNCTION(BlueprintCallable, Category = "WUSettings")
	void ResetCategoryToDefaults(EWUSettingCategory Category);
	
	// Broadcast when ApplyConfirm settings were applied and need confirmation (also when the timer restarts).
	UPROPERTY(BlueprintAssignable, Category="WUSettings")
	FWUOnConfirmRequired OnConfirmRequired;

	// Broadcast when pending settings are confirmed (true) or reverted (false, including timeout).
	UPROPERTY(BlueprintAssignable, Category="WUSettings")
	FWUOnPendingResolved OnPendingResolved;

	// Fired after a value is set or reverted, so UI rows can refresh.
	FWUOnSettingChanged OnSettingChanged;

private:
	void LoadSettings();
	void ApplySetting(UWorld* World, const FWUSettingDescriptor& Descriptor, const FWUSettingValue& Value) const;

	void StartConfirmTimer();
	void StopConfirmTimer();
	bool HandleConfirmTimeout(float DeltaTime);

	// False on type mismatch or non-finite float. Float/Int are clamped to the descriptor range
	static bool SanitizeValue(const FWUSettingDescriptor& Descriptor, const FWUSettingValue& InValue, FWUSettingValue& OutValue);

	UPROPERTY()
	TObjectPtr<UWUSettingSaveGame> SaveGameObject;

	UPROPERTY()
	TMap<FName, FWUSettingValue> CachedValues;

	// Value each pending ApplyConfirm setting had before its first unconfirmed change
	TMap<FName, FWUSettingValue> PendingRollback;

	// Real-time ticker (not TimerManager): world timers freeze while the game is paused
	// and settings menus usually pause the game.
	FTSTicker::FDelegateHandle ConfirmTickerHandle;
	double ConfirmDeadline = 0.0;

	static constexpr const TCHAR* SaveSlot = TEXT("WUSettings");
	static constexpr int32 UserIndex = 0;
};
