// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUVideoSettingApplier.h"
#include "WUSettingsRuntime/Appliers/WUSettingsApplierRegistry.h"

static struct FWURegisterVideoApplier
{
	FWURegisterVideoApplier()
	{
		FWUSettingsApplierRegistry::Get().RegisterApplier(MakeUnique<FWUVideoSettingApplier>());
	}
} GRegisterVideoApplier;
