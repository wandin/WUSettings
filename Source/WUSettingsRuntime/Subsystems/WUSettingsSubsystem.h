// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WUSettingsRuntime/Persistence/WUSettingSaveGame.h"
#include "WUSettingsRuntime/Persistence/WUSettingValue.h"
#include "WUSettingsSubsystem.generated.h"

UCLASS()
class WUSETTINGSRUNTIME_API UWUSettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Returns the saved value, or the descriptor default. False if the setting is unknown.
	bool GetSettingValue(FName SettingName, FWUSettingValue& OutValue) const;

	// Validates (known name, matching type), clamps to Min/Max, applies and saves
	// Returns false if the value was rejected.
	bool SetSettingValue(FName SettingName, const FWUSettingValue& Value);

	void SaveSettings();

	// Called by UWUSettingsWorldSubsystem when a game world begins play.
	void ApplyAllRegisteredSettings(UWorld* World);

private:
	void LoadSettings();
	void ApplySetting(UWorld* World, const FWUSettingDescriptor& Descriptor, const FWUSettingValue& Value) const;

	// False on type mismatch or non-finite float. Float/Int are clamped to the descriptor range
	static bool SanitizeValue(const FWUSettingDescriptor& Descriptor, const FWUSettingValue& InValue, FWUSettingValue& OutValue);
	
	UPROPERTY()
	TObjectPtr<UWUSettingSaveGame> SaveGameObject;

	UPROPERTY()
	TMap<FName, FWUSettingValue> CachedValues;

	static constexpr const TCHAR* SaveSlot = TEXT("WUSettings");
	static constexpr int32 UserIndex = 0;
};
