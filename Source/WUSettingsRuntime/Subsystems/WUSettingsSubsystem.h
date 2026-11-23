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

	// Get / Set
	bool GetSettingValue(FName SettingName, FWUSettingValue& OutValue);
	void SetSettingValue(FName SettingName, const FWUSettingValue& Value);

	// Persistence
	void SaveSettings();

	void ApplyAllRegisteredSettings();

	UFUNCTION(Exec)
	void SetAudioVolume(const FString& SettingName, float Value);

private:
	void LoadSettings();

	UPROPERTY()
	UWUSettingSaveGame* SaveGameObject = nullptr;

	UPROPERTY()
	TMap<FName, FWUSettingValue> CachedValues;

	static constexpr const TCHAR* SaveSlot = TEXT("WUSettings");
};
