

#include "WUVideoSettingApplier.h"
#include "WuSettingsRuntime/Appliers/WUSettingsApplierRegistry.h"

static struct FWURegisterVideoApplier
{
	FWURegisterVideoApplier()
	{
		FWUSettingsApplierRegistry::Get().RegisterApplier(MakeUnique<FWUVideoSettingApplier>());
	}
} GRegisterVideoApplier;
