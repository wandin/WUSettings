// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUSettingsRuntime.h"

#include "WUSettingsRuntime/Appliers/WUSettingsApplierRegistry.h"
#include "WUSettingsRuntime/Appliers/Audio/WUAudioSettingApplier.h"
#include "WUSettingsRuntime/Appliers/Video/WUVideoSettingApplier.h"
#include "WUSettingsRuntime/Defaults/WUSettingsDefaults.h"
#include "WUSettingsRuntime/Logging/WUSettingsLog.h"
#include "WUSettingsRuntime/SettingsCore/WUSettingsRegistry.h"

#define LOCTEXT_NAMESPACE "FWUSettingsRuntimeModule"

void FWUSettingsRuntimeModule::StartupModule()
{
    // Built-in settings
    WUSettingsDefaults::RegisterAudioSettings();
    WUSettingsDefaults::RegisterVideoSettings();

    // Built-in appliers
    FWUSettingsApplierRegistry& Appliers = FWUSettingsApplierRegistry::Get();
    Appliers.RegisterApplier(MakeUnique<FWUAudioSettingApplier>());
    Appliers.RegisterApplier(MakeUnique<FWUVideoSettingApplier>());

    UE_LOG(LogWUSettings, Log, TEXT("WUSettingsRuntime started: %d settings registered"), FWUSettingsRegistry::Get().GetAllSettings().Num());
}

void FWUSettingsRuntimeModule::ShutdownModule()
{
    FWUSettingsApplierRegistry::Get().Reset();
    FWUSettingsRegistry::Get().Reset();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FWUSettingsRuntimeModule, WUSettingsRuntime)