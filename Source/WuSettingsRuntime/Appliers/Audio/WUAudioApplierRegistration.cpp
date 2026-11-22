#include "WUAudioSettingApplier.h"
#include "WuSettingsRuntime/Appliers/WUSettingsApplierRegistry.h"

static struct FWURegisterAudioApplier
{
	FWURegisterAudioApplier()
	{
		FWUSettingsApplierRegistry::Get().RegisterApplier(MakeUnique<FWUAudioSettingApplier>());
	}
} GRegisterAudioApplier;
