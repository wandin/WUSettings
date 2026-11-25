// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#include "WUVideoResolutions.h"

#include "Kismet/KismetSystemLibrary.h"

namespace
{
	void SortAndDedupe(TArray<FIntPoint>& Resolutions)
	{
		TArray<FIntPoint> Unique;
		Unique.Reserve(Resolutions.Num());
		for (const FIntPoint& Res : Resolutions)
		{
			Unique.AddUnique(Res);
		}

		Unique.Sort([](const FIntPoint& A, const FIntPoint& B)
		{
			return A.X != B.X ? A.X < B.X : A.Y < B.Y;
		});

		Resolutions = MoveTemp(Unique);
	}
}

TArray<FIntPoint> WUVideoResolutions::GetSupportedFullscreenResolutions()
{
	TArray<FIntPoint> Resolutions;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
	SortAndDedupe(Resolutions);
	return Resolutions;
}

TArray<FIntPoint> WUVideoResolutions::GetWindowedResolutions()
{
	TArray<FIntPoint> Resolutions;
	UKismetSystemLibrary::GetConvenientWindowedResolutions(Resolutions);
	SortAndDedupe(Resolutions);
	return Resolutions;
}

bool WUVideoResolutions::IsSupportedFullscreenResolution(const FIntPoint& Resolution)
{
	const TArray<FIntPoint> Supported = GetSupportedFullscreenResolutions();
	return Supported.IsEmpty() || Supported.Contains(Resolution);
}
