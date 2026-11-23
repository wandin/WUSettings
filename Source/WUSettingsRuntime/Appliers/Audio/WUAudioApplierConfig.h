// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WUAudioApplierConfig.generated.h"

USTRUCT(BlueprintType)
struct FWUAudioSettingSoundClassMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SettingName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USoundClass> SoundClass;
};


UCLASS(BlueprintType)
class WUSETTINGSRUNTIME_API UWUAudioApplierConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	// Mix that will receive classe's overrides.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USoundMix> SoundMix;

	// List (easy to edit in editor)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWUAudioSettingSoundClassMap> Maps;

	// Defaults for overrides
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FadeInTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FadeOutTime = 0.1f;
};
