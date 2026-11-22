#include "WUSettingsApplierRegistry.h"

FWUSettingsApplierRegistry& FWUSettingsApplierRegistry::Get()
{
	static FWUSettingsApplierRegistry Instance;
	return Instance;
}

void FWUSettingsApplierRegistry::RegisterApplier(TUniquePtr<IWUSettingApplier> Applier)
{
	Appliers.Add(MoveTemp(Applier));
}

IWUSettingApplier* FWUSettingsApplierRegistry::FindApplier(const FWUSettingDescriptor& Descriptor)
{
	for (const TUniquePtr<IWUSettingApplier>& Applier : Appliers)
	{
		if (Applier->CanApply(Descriptor))
		{
			return Applier.Get();
		}
	}
	return nullptr;
}
