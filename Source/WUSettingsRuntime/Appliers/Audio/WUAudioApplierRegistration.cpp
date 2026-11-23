// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUAudioSettingApplier.h"
#include "WUSettingsRuntime/Appliers/WUSettingsApplierRegistry.h"

static struct FWURegisterAudioApplier
{
	FWURegisterAudioApplier()
	{
		FWUSettingsApplierRegistry::Get().RegisterApplier(MakeUnique<FWUAudioSettingApplier>());
	}
} GRegisterAudioApplier;
