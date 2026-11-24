// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WUSettingsWorldSubsystem.generated.h"

// Re-applies all settings when a game world begins play.
// UWUSettingsSubsystem::Initialize runs before any world exists, so settings that need
// a world or a PlayerController (sound mixes, FOV) must be applied here.
UCLASS()
class WUSETTINGSRUNTIME_API UWUSettingsWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
};