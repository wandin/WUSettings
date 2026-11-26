// Copyright (c) 2026 Wanderson Lopes. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Queries for the resolutions the current monitor supports.
 * Used by the video applier to validate values, and meant to feed the UI dropdowns
 * so players can only pick valid options.
 */
namespace WUVideoResolutions
{
	// Exclusive-fullscreen modes supported by the current monitor. Sorted ascending, no duplicates.
	WUSETTINGSRUNTIME_API TArray<FIntPoint> GetSupportedFullscreenResolutions();

	// Window sizes that fit on the current desktop. Sorted ascending, no duplicates.
	WUSETTINGSRUNTIME_API TArray<FIntPoint> GetWindowedResolutions();

	// True if Resolution is a supported fullscreen mode.
	// Also true when the list can't be queried (e.g. null RHI), so we never block a value we can't verify.
	WUSETTINGSRUNTIME_API bool IsSupportedFullscreenResolution(const FIntPoint& Resolution);

	// Refresh rates (Hz) the monitor supports at Resolution. Sorted ascending, no duplicates.
	// Empty when unknown (e.g. no RHI). Informational only: UE lets DXGI pick the fullscreen refresh rate,
	// so use this to offer frame rate limits options (e.g. "Match monitor"), not to switch display modes.
	WUSETTINGSRUNTIME_API TArray<int32> GetSupportedRefreshRates(const FIntPoint& Resolution);
	
}
