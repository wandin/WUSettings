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

	UPROPERTY()
	int32 Version = 1;

	UPROPERTY()
	TMap<FName, FWUSettingValue> StoredSettings;
};
