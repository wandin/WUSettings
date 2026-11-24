// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WUSettingValue.h"
#include "GameFramework/SaveGame.h"
#include "WUSettingSaveGame.generated.h"

UCLASS()
class WUSETTINGSRUNTIME_API UWUSettingSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	static constexpr int32 CurrentVersion = 1;
	
	UPROPERTY()
	int32 Version = CurrentVersion;

	UPROPERTY()
	TMap<FName, FWUSettingValue> StoredSettings;
};
