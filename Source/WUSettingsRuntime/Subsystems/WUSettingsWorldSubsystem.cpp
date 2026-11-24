// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUSettingsWorldSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "WUSettingsSubsystem.h"

bool UWUSettingsWorldSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UWUSettingsWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	UGameInstance* GameInstance = InWorld.GetGameInstance();
	if (UWUSettingsSubsystem* Settings = GameInstance ? GameInstance->GetSubsystem<UWUSettingsSubsystem>() : nullptr)
	{
		Settings->ApplyAllRegisteredSettings(&InWorld);
	}
}