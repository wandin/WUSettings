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

Registered settings (9):
- `Audio.MasterVolume`, `MusicVolume`, `SFXVolume`, `VoiceVolume`, `UIVolume`: Float 0–1, default 1.0.
- `Video.VSync`: Bool.
- `Video.FOV`: Float 60–120, default 90.
- `Video.WindowMode`: Enum 0–2 (Windowed/Borderless/Fullscreen), default Borderless, ApplyConfirm. It is applied immediately until Phase 7.
- `Video.Resolution`: Resolution (`FIntPoint`). `(0,0)` means native desktop resolution, which is the default. ApplyConfirm, applied immediately until Phase 7.

## Testing notes

- Window mode and resolution can't be tested in PIE. Use **Standalone Game**.
- The Standalone log does not appear in the editor Output Log. Add `-log` to Editor Preferences → Play → Additional Launch Parameters, or read `Saved/Logs/WUSettingsSample.log`.
- The editor launches Standalone with `-windowed -ResX/-ResY`, so it always starts in a small window and switches to the saved mode on BeginPlay. Packaged builds start directly in the saved mode.

## Roadmap

Next: **Video Display**, in this order:
1. ~~Window Mode~~ (done in Phase 4).
2. ~~Resolution~~ (done in Phase 5).
3. Refresh Rate. In Phase 4, exclusive Fullscreen at the native 1920×1080 looked blurry and triggered an OS/driver notice. The likely cause is a refresh rate mismatch, which needs to be checked.
4. Apply/Confirm flow with timed Rollback, for `EWUSettingApplyStrategy::ApplyConfirm`.

After that:
- **Gameplay:** FOV ownership. Apply on possess or on PlayerController ready, so it also works for network clients whose PC arrives after BeginPlay. Decide whether FOV belongs in Video or Gameplay.
- **UI module** (`WUSettingsUI`, runtime UMG): C++ logic, Blueprint only for styling. Add a debounced save or an explicit Apply button, so sliders don't write the `.sav` on every tick.
- **Remaining categories:** Graphics (scalability), Input (rebinding through EnhancedInput), Accessibility, Network, Developer.
- **Editor module** (`WUSettingsEditor`): tooling, currently empty.
- **Deferred audio:** output device, dynamic range, spatialization and occlusion.
- **Fab readiness:** docs, sample content, README, version bump out of beta.

Known limitations and tech debt:
- Int and Float descriptors must set `MinValue` and `MaxValue`. The defaults are 0–1 and values get clamped to that range.
- A `.sav` from a newer plugin version is ignored, then overwritten on the next save.
- `UWUAudioApplierConfig::FadeOutTime` is unused.
- In Windowed mode, a resolution larger than the desktop produces a window bigger than the screen. The UI should only offer `WUVideoResolutions::GetWindowedResolutions()`.
- The sample PlayerController still logs to `LogTemp`. That is fine, since it is not part of the plugin.
