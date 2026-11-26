// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WUSettingsRuntime/Appliers/Audio/WUAudioApplierConfig.h"
#include "WUSettingsDeveloperSettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="WU Settings"))
class WUSETTINGSRUNTIME_API UWUSettingsDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	// Audio
	UPROPERTY(Config, EditAnywhere, Category="Audio")
	TSoftObjectPtr<UWUAudioApplierConfig> AudioApplierConfig;

	// Video
	// Seconds the player has to confirm ApplyConfirm settings (window mode, resolution) before they revert.
	UPROPERTY(Config, EditAnywhere, Category="Video", meta=(ClampMin="5", ClampMax="60", Units="Seconds"))
	float ConfirmTimeoutSeconds = 15.f;
};
