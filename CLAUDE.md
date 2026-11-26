# CLAUDE.md

Context for Claude (and any other assistant) working on **WUSettings**. Read this file and [GUIDELINE.md](GUIDELINE.md) before suggesting any change.

## What this is

WUSettings is a modular, data-driven settings framework plugin for **Unreal Engine 5.8**. It covers Audio, Video, Graphics, Gameplay, Input, Accessibility, Network and Developer settings. It is built for the author's own projects, as a portfolio piece, and for a future sale on Fab.

- This repository is **the plugin only**: github.com/wandin/WUSettings, branch `main`.
- It lives inside the host project `WUSettingsSample` at `Plugins/WUSettings`. The host project is not versioned and is used only for local testing. It has a PlayerController with a debug Exec command, `WU_SetSetting <Name> <Value>`.
- The engine is installed at `W:\UE_5.8`. The host targets use `BuildSettingsVersion.V7` and `EngineIncludeOrderVersion.Latest`.

## How we work (important)

- **The author writes the code.** Do not edit source files (`.h`, `.cpp`, `.Build.cs`, `.uplugin`) unless explicitly asked. Deliver the code and steps instead.
- Deliver work in **small phases**, each ending in a **compile checkpoint** that has concrete things to verify, such as log lines and in-game behavior. Wait for "Fase N ok" before starting the next phase.
- Give full file contents for new or rewritten files, and exact before/after snippets with paths for small edits.
- Before writing code that uses an engine API you are unsure about, check it against the engine source at `W:\UE_5.8\Engine\Source`.
- When files are added or removed, remind the author to regenerate the project files.
- Commits: `git add -u` plus explicit `git add <new files>`, never `git add .`, because test assets must stay out. Commit after each phase passes.
- The author writes in Portuguese (pt-BR). Reply in Portuguese. Code, comments, logs and docs are in English.

## Current status

Done:
- Phase 1: migrated to UE 5.8 and renamed the core module to `WUSettingsRuntime`.
- Phase 2: explicit registration in `StartupModule` (no static auto-registration), registry validation, `Reset()` on shutdown.
- Phase 3: persistence fixed. `.sav` is written and loaded. Settings are re-applied on world BeginPlay through `UWUSettingsWorldSubsystem`. Values are validated and clamped. VSync uses `ApplyNonResolutionSettings`. The audio applier holds strong references and pushes the mix once per world.
- Phase 4: Window Mode applier (`EWUWindowMode` translated to `EWindowMode`), Enum range validation, and the video `CanApply` now filters by category only. Window mode is skipped in PIE. The idempotency check compares against `GSystemResolution.WindowMode`, the actual window state. The mode is mirrored to GameUserSettings.ini through `ConfirmVideoMode` and `SaveSettings`.
- Phase 5: Resolution setting.
  - `FWUSettingValue::ResolutionValue` and `MakeResolution` / `MakeNativeResolution`. The unused `FWUResolution` struct was removed.
  - WindowMode and Resolution each set their part in GameUserSettings, then `ApplyDisplaySettings` applies the combined state once, idempotently against `GSystemResolution`.
  - In Fullscreen, an unsupported resolution falls back to the desktop resolution with a Warning. The saved value is kept.
  - `WUVideoResolutions` (in `Video/`) lists supported fullscreen modes and windowed sizes, for validation now and the UI later.
- Phase 6: Frame Rate Limit (replaces the planned "Refresh Rate" setting).
  - UE 5.8's D3D12 RHI always passes `RefreshRate = 0/0` to DXGI (`WindowsD3D12Viewport.cpp`, `D3D12Viewport.cpp`), so DXGI picks the fullscreen refresh rate itself. A plugin can't select the monitor Hz without engine changes.
  - `Video.FrameRateLimit` goes through `UGameUserSettings::SetFrameRateLimit` + `ApplyNonResolutionSettings` (the `t.MaxFPS` cvar). Any non-zero value below 30 is raised to 30 by the applier. The saved value is kept.
  - `WUVideoResolutions::GetSupportedRefreshRates(Resolution)` (through `RHIGetAvailableResolutions`, guarded against a null `GDynamicRHI`) lists the monitor Hz for UI presets such as "Match monitor". It is informational only. The `RHI` module was added as a private dependency.

- Phase 7: Apply/Confirm flow with timed Rollback.
  - `ApplyConfirm` settings are applied immediately but not saved. The value from before the first unconfirmed change goes into `PendingRollback`, and a countdown starts (`UWUSettingsDeveloperSettings::ConfirmTimeoutSeconds`, default 15 s). Each new pending change restarts it.
  - `ConfirmPendingSettings()` saves. `RevertPendingSettings()`, or the timeout, re-applies the rollback values. If there is no world, the value is saved directly, because nothing was applied.
  - `SaveSettings` writes rollback values in place of pending ones, so the `.sav` never holds unconfirmed values. Quitting while pending therefore reverts on the next boot.
  - The countdown uses `FTSTicker` (real time), because world and GameInstance timers freeze while the game is paused.
  - The Blueprint API is `OnConfirmRequired(TimeoutSeconds)`, `OnPendingResolved(bConfirmed)`, `HasPendingConfirmation()` and `GetConfirmTimeRemaining()`.
  - The sample has `WU_Confirm` and `WU_Revert` Exec commands.
- Phase 8: UI base (row widgets).
  - Runtime: `FWUSettingDescriptor::EnumOptions`, where each label's index is the enum value. WindowMode fills it from the UENUM DisplayNames.
  - Runtime: the native event `UWUSettingsSubsystem::OnSettingChanged(FName)` fires on set and on revert. `FWUSettingDescriptor` and `FWUSettingValue` are exported.
  - Runtime: `LogWUSettingsUI` now lives in the UI module (`WUSettingsUI/Logging`). The UI module depends publicly on `WUSettingsRuntime`.
  - UI rows (`WUSettingsUI/Rows`):
    - `UWUSettingRow` (abstract base) binds by `SettingName` on construct, keeps a descriptor copy, and refreshes on `OnSettingChanged`.
    - `UWUSettingRowBool` uses a `CheckBox`.
    - `UWUSettingRowSlider` handles Float and Int with a `Slider` and an optional `ValueText`. It snaps to Step, previews while dragging, commits on release, and commits each keyboard/gamepad step.
    - `UWUSettingRowEnum` uses a `ComboBox` and ignores `ESelectInfo::Direct`.
    - Blueprint children only provide layout through `BindWidget`.
  - Test widgets live in the Sample at `Content/WUSettingsTest/`: `WBP_TestRow*` and `WBP_TestSettings`, shown from the Level Blueprint.

Registered settings (10):
- `Audio.MasterVolume`, `MusicVolume`, `SFXVolume`, `VoiceVolume`, `UIVolume`: Float 0–1, default 1.0.
- `Video.VSync`: Bool.
- `Video.FrameRateLimit`: Int 0–360, where 0 means unlimited (the default). Immediate.
- `Video.FOV`: Float 60–120, default 90.
- `Video.WindowMode`: Enum 0–2 (Windowed/Borderless/Fullscreen), default Borderless, ApplyConfirm.
- `Video.Resolution`: Resolution (`FIntPoint`). `(0,0)` means native desktop resolution, which is the default. ApplyConfirm.

## Testing notes

- Window mode and resolution can't be tested in PIE. Use **Standalone Game**.
- The Standalone log does not appear in the editor Output Log. Add `-log` to Editor Preferences → Play → Additional Launch Parameters, or read `Saved/Logs/WUSettingsSample.log`.
- The editor launches Standalone with `-windowed -ResX/-ResY`, so it always starts in a small window and switches to the saved mode on BeginPlay. Packaged builds start directly in the saved mode.
- Frame rate can be tested in PIE with `stat fps`. Turn VSync off first, because it caps FPS at the monitor refresh rate. With `FrameRateLimit 0` the editor still caps at about 120 FPS (`SmoothedFrameRateRange` in `[/Script/UnrealEd.EditorEngine]`). Standalone and packaged builds have no such cap.

## Roadmap

Phases 1–8 are done. Phases 4–7 covered Video Display: Window Mode, Resolution, Frame Rate Limit, and Apply/Confirm.

The UI comes before the remaining categories. It is data-driven (rows are built from the Registry), so later settings appear in the menu automatically. Building it now also shows what the descriptors are missing.

| Phase | Content |
|---|---|
| ~~8~~ | (Done) UI base. Runtime prep: Enum option labels in the descriptor, an `OnSettingChanged` event, API exports, and `LogWUSettingsUI` moved to the UI module. Row widgets: a base row plus Bool (checkbox), Slider (Float/Int; it commits on release, so the `.sav` isn't written on every tick) and Enum (combo box). |
| **9 (next)** | UI screen. Resolution row (options depend on the window mode), a screen that builds rows per category from the Registry, the confirm popup with a countdown, and Reset to defaults. |
| 10 | Graphics: overall quality preset, scalability groups, resolution scale. |
| 11 | Gameplay: FOV applied on possess (works for network clients), mouse sensitivity and invert. Decide whether FOV belongs in Video or Gameplay. |
| 12–13 | Input: rebinding through EnhancedInput (`UEnhancedInputUserSettings`) and the rebinding UI. |
| 14 | Accessibility: subtitles, colorblind mode, text size. |
| 15 | Network and Developer: stats and debug options. |
| 16 | Editor module (`WUSettingsEditor`): tooling such as descriptor validation. |
| 17 | Fab readiness: docs, sample content, README, version bump out of beta. |

The order from Phase 10 on can change. Large blocks (UI, Input) may split into more phases.

Deferred, with no phase yet: `RequiresRestart` handling, and audio output device, dynamic range, spatialization and occlusion.

Known limitations and tech debt:
- Int and Float descriptors must set `MinValue` and `MaxValue`. The defaults are 0–1 and values get clamped to that range.
- A `.sav` from a newer plugin version is ignored, then overwritten on the next save.
- `UWUAudioApplierConfig::FadeOutTime` is unused.
- In Windowed mode, a resolution larger than the desktop produces a window bigger than the screen. The UI should only offer `WUVideoResolutions::GetWindowedResolutions()`.
- Reverting WindowMode and Resolution together can switch the window twice, because each one calls `ApplyDisplaySettings`. A future fix is to batch display appliers.
- Display settings are mirrored to GameUserSettings.ini even while pending. Quitting before confirming boots once in the unconfirmed mode, and then the `.sav` switches it back on BeginPlay.
- `RequiresRestart` is treated like `Immediate`.
- The sample PlayerController still logs to `LogTemp`. That is fine, since it is not part of the plugin.
